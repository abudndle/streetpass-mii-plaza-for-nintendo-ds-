#!/usr/bin/env python3
"""Privacy-safe CECD Plaza bookkeeping and message-header inspector.

The caller must provide an IVFC-authenticated, fail-closed SAVE reader.
This module never reads message bodies, sender identifiers, or Mii payloads.
"""
import struct
from cecd_mailbox_metadata import inspect as metadata_inspect, u32, u64

MAX_ENTRIES = 10000
MAX_TABLE_BYTES = 256 * 1024
MAX_MESSAGE_BYTES = 16 * 1024 * 1024


def parse_box_info(header, file_size):
    if len(header) != 32 or struct.unpack_from('<H', header)[0] != 0x6262:
        raise ValueError('invalid BoxInfo header')
    (length, maximum, used, capacity, count, batch, message_max) = struct.unpack_from('<7I', header, 4)
    if (length != file_size or count > capacity or capacity > MAX_ENTRIES or
            file_size != 32 + count * 112 or used > maximum or
            batch > maximum or message_max > MAX_MESSAGE_BYTES):
        raise ValueError('inconsistent BoxInfo bookkeeping')
    return {'message_count': count, 'current_box_bytes': used}


def parse_message_header(header, file_size):
    if len(header) != 112 or struct.unpack_from('<H', header)[0] != 0x6060:
        raise ValueError('invalid CECD message header')
    total, headers, body = struct.unpack_from('<III', header, 4)
    if (file_size < 144 or file_size > MAX_MESSAGE_BYTES or
            total != file_size or headers < 112 or headers > total - 32 or
            headers + body + 32 != total):
        raise ValueError('inconsistent message length')
    return {'message_bytes': total, 'header_bytes': headers,
            'body_bytes': body, 'trailer_bytes': 32}


def inspect(read_authenticated):
    """Return only public Plaza counts from authenticated SAVE metadata."""
    meta = metadata_inspect(read_authenticated)
    if not meta['plaza_mailbox_identifier_present']:
        raise ValueError('Plaza mailbox absent')
    h = read_authenticated(0, 0x88)
    block_size, data_at, blocks = u32(h, 0x24), u64(h, 0x58), u32(h, 0x60)
    fat_at = u64(h, 0x48)
    if u32(h, 0x50) != blocks:
        raise ValueError('SAVE FAT size mismatch')

    def table(at, entry_size):
        start, length = u32(h, at), u32(h, at + 4)
        count = u32(read_authenticated(data_at + start * block_size, 4), 0)
        if (count < 2 or count > MAX_ENTRIES or
                count * entry_size > length * block_size or
                length * block_size > MAX_TABLE_BYTES):
            raise ValueError('invalid SAVE table size')
        raw = read_authenticated(data_at + start * block_size,
                                 count * entry_size)
        if len(raw) != count * entry_size:
            raise ValueError('truncated SAVE table')
        return raw, count

    dirs, nd = table(0x68, 40)
    files, nf = table(0x78, 48)

    def prefix(file_entry, length):
        first, size = u32(file_entry, 0x1c), u64(file_entry, 0x20)
        if (first == 0x80000000 or first >= blocks or size < length or
                size > MAX_MESSAGE_BYTES or length > block_size):
            raise ValueError('invalid file header extent')
        fat = read_authenticated(fat_at + 8 * (first + 1), 8)
        if len(fat) != 8 or not (u32(fat, 0) & 0x80000000):
            raise ValueError('file start not allocated')
        data = read_authenticated(data_at + first * block_size, length)
        if len(data) != length:
            raise ValueError('unauthenticated file prefix')
        return data

    boxes = {name: {'present': False, 'info': None, 'messages': 0, 'bytes': 0}
             for name in ('inbox', 'outbox')}
    stack = [(1, 0, ())]
    visited = set()
    while stack:
        index, parent, route = stack.pop()
        if index == 0:
            continue
        if index >= nd or index in visited:
            raise ValueError('invalid directory cycle')
        visited.add(index)
        ent = dirs[index * 40:(index + 1) * 40]
        if u32(ent, 0) != parent:
            raise ValueError('directory parent mismatch')
        name = ent[4:20].split(b'\0', 1)[0]
        if index == 1:
            path = ()
        elif parent == 1 and name == b'CEC':
            path = ('CEC',)
        elif route == ('CEC',) and name == b'00020800':
            path = ('CEC', 'PLAZA')
        elif route == ('CEC', 'PLAZA') and name == b'InBox___':
            path = ('CEC', 'PLAZA', 'inbox')
        elif route == ('CEC', 'PLAZA') and name == b'OutBox__':
            path = ('CEC', 'PLAZA', 'outbox')
        else:
            path = ()
        if len(path) == 3:
            box = boxes[path[2]]
            if box['present']:
                raise ValueError('duplicate Plaza mailbox directory')
            box['present'] = True
            fidx = u32(ent, 0x1c)
            while fidx:
                if fidx >= nf:
                    raise ValueError('invalid file index')
                file = files[fidx * 48:(fidx + 1) * 48]
                if u32(file, 0) != index:
                    raise ValueError('file parent mismatch')
                label = file[4:20].split(b'\0', 1)[0]
                size = u64(file, 0x20)
                if label == b'BoxInfo_____':
                    if box['info'] is not None:
                        raise ValueError('duplicate BoxInfo')
                    box['info'] = parse_box_info(prefix(file, 32), size)
                elif len(label) in (12, 13) and label.startswith(b'_'):
                    msg = parse_message_header(prefix(file, 112), size)
                    box['messages'] += 1
                    box['bytes'] += msg['message_bytes']
                fidx = u32(file, 0x14)
        if index != 1 and u32(ent, 0x14):
            stack.append((u32(ent, 0x14), parent, route))
        if u32(ent, 0x18):
            stack.append((u32(ent, 0x18), index, path))
    for box in boxes.values():
        if box['present'] and (
                box['info'] is None or
                box['info']['message_count'] != box['messages'] or
                box['info']['current_box_bytes'] != box['bytes']):
            raise ValueError('BoxInfo and message files disagree')
    return {
        'format': 'CECD Plaza',
        'inbox': {'present': boxes['inbox']['present'],
                  'message_count': boxes['inbox']['messages']},
        'outbox': {'present': boxes['outbox']['present'],
                   'message_count': boxes['outbox']['messages']},
        'headers_only': True,
        'message_bodies_read': False,
        'sender_ids_exposed': False,
        'full_ivfc_verified': False,
        'native_stock_3ds_communication_verified': False,
    }
