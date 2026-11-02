#!/usr/bin/env python3
"""Read-only CECD SAVE metadata inspection using an authenticated reader.

The supplied callback MUST reject any byte range not independently
verified against IVFC. No file payloads or unknown names are ever exported.
"""
import struct

DIRECTORY_LABELS = ('CEC', 'InBox___', 'OutBox__')
FILE_LABELS = ('MBoxList____', 'MBoxInfo____', 'BoxInfo_____',
               'OBIndex_____', 'MacFilter___', 'eventlog.dat')
MAX_TABLE_READ = 256 * 1024
MAX_ENTRIES = 10000
PLAZA_MAILBOX = b'00020800'


def u32(buf, at):
    return struct.unpack_from('<I', buf, at)[0]


def u64(buf, at):
    return struct.unpack_from('<Q', buf, at)[0]


def classify(raw, labels):
    """Return only allowlisted labels; never reveal unknown names."""
    if len(raw) != 16:
        raise ValueError('invalid name field')
    name = raw.split(b'\0', 1)[0]
    for allowed in labels:
        if name == allowed.encode('ascii'):
            return allowed
    return None


def inspect(read_authenticated):
    """Inspect metadata only; read_authenticated(offset,length) fails closed."""
    h = read_authenticated(0, 0x88)
    if (len(h) != 0x88 or h[:4] != b'SAVE' or
            u32(h, 4) != 0x40000 or u64(h, 8) != 0x20):
        raise ValueError('unsupported SAVE header')
    block_size = u32(h, 0x24)
    if not (512 <= block_size <= 65536) or block_size & (block_size - 1):
        raise ValueError('invalid SAVE data block size')
    data_at = u64(h, 0x58)
    data_blocks = u32(h, 0x60)

    def table(start_at, blocks_at, max_at, entry_size):
        start = u32(h, start_at)
        blocks = u32(h, blocks_at)
        maximum = u32(h, max_at)
        length = blocks * block_size
        if (not blocks or length > MAX_TABLE_READ or maximum > MAX_ENTRIES or
                start + blocks > data_blocks):
            raise ValueError('unbounded SAVE metadata table')
        offset = data_at + start * block_size
        prefix = read_authenticated(offset, 8)
        if len(prefix) != 8:
            raise ValueError('truncated SAVE metadata table prefix')
        count, capacity = u32(prefix, 0), u32(prefix, 4)
        if (count < 2 or count > capacity or capacity > maximum + 2 or
                count * entry_size > length):
            raise ValueError('invalid SAVE metadata entry count')
        contents = read_authenticated(offset, count * entry_size)
        if len(contents) != count * entry_size:
            raise ValueError('truncated SAVE metadata table')
        return contents, count

    dirs, ndirs = table(0x68, 0x6c, 0x70, 40)
    files, nfiles = table(0x78, 0x7c, 0x80, 48)
    directory_counts = dict.fromkeys(DIRECTORY_LABELS, 0)
    file_counts = dict.fromkeys(FILE_LABELS, 0)
    visited_dirs, visited_files = set(), set()
    plaza_found = False
    plaza_inbox = False
    plaza_outbox = False
    plaza_inbox_entries = 0
    plaza_outbox_entries = 0

    # Iterative traversal limits attacker-controlled nesting to table size.
    # Route includes public directory labels only.
    stack = [(1, 0, ())]
    while stack:
        index, parent, route = stack.pop()
        if not index:
            continue
        if index >= ndirs or index in visited_dirs:
            raise ValueError('invalid or cyclic directory link')
        visited_dirs.add(index)
        entry = dirs[index * 40:(index + 1) * 40]
        if u32(entry, 0) != parent:
            raise ValueError('directory parent mismatch')
        raw_name = entry[4:20].split(b'\0', 1)[0]
        recognized = classify(entry[4:20], DIRECTORY_LABELS)
        if recognized:
            directory_counts[recognized] += 1

        if index == 1:
            child_route = ()
        elif parent == 1 and raw_name == b'CEC':
            child_route = ('CEC',)
        elif route == ('CEC',) and raw_name == PLAZA_MAILBOX:
            child_route = ('CEC', 'PLAZA')
            plaza_found = True
        elif route == ('CEC', 'PLAZA') and raw_name == b'InBox___':
            child_route = ('CEC', 'PLAZA', 'IN')
            plaza_inbox = True
        elif route == ('CEC', 'PLAZA') and raw_name == b'OutBox__':
            child_route = ('CEC', 'PLAZA', 'OUT')
            plaza_outbox = True
        else:
            child_route = ()

        if index != 1:
            stack.append((u32(entry, 0x14), parent, route))
        stack.append((u32(entry, 0x18), index, child_route))
        file_index = u32(entry, 0x1c)
        while file_index:
            if file_index >= nfiles or file_index in visited_files:
                raise ValueError('invalid or cyclic file link')
            visited_files.add(file_index)
            file_entry = files[file_index * 48:(file_index + 1) * 48]
            if u32(file_entry, 0) != index:
                raise ValueError('file parent mismatch')
            label = classify(file_entry[4:20], FILE_LABELS)
            if label:
                file_counts[label] += 1
            if child_route == ('CEC', 'PLAZA', 'IN'):
                plaza_inbox_entries += 1
            elif child_route == ('CEC', 'PLAZA', 'OUT'):
                plaza_outbox_entries += 1
            file_index = u32(file_entry, 0x14)

    return {
        'format': 'SAVE',
        'reachable_directories_including_root': len(visited_dirs),
        'reachable_files': len(visited_files),
        'standard_directory_counts': directory_counts,
        'standard_file_counts': file_counts,
        'plaza_mailbox_identifier_present': plaza_found,
        'plaza_inbox_directory_present': plaza_inbox,
        'plaza_outbox_directory_present': plaza_outbox,
        'plaza_inbox_file_entries': plaza_inbox_entries,
        'plaza_outbox_file_entries': plaza_outbox_entries,
        'private_names_exposed': False,
        'file_payloads_read': False,
        'full_ivfc_verified': False,
        'native_3ds_streetpass_verified': False,
    }
