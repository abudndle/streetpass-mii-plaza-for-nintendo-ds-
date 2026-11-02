#include "storage.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static int path_join(char *out, size_t cap, const char *base, const char *file) {
    if (!out || !base || !file) return -1;
    int n = snprintf(out, cap, "%s/%s", base, file);
    return n < 0 || (size_t)n >= cap ? -1 : 0;
}
static int ensure_dir(const char *p) {
    return mkdir(p, 0777) == 0 || errno == EEXIST ? 0 : -1;
}
int spds_storage_init(const char *base) {
    char path[SPDS_PATH_MAX];
    if (!base || !*base || ensure_dir(base)) return -1;
    if (path_join(path, sizeof(path), base, "inbox") || ensure_dir(path))
        return -1;
    if (path_join(path, sizeof(path), base, "outbox") || ensure_dir(path))
        return -1;
    return 0;
}
static int read_packet(const char *path, SpdsRecord *record) {
    uint8_t data[SPDS_WIRE_SIZE];
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    size_t n = fread(data, 1, sizeof(data), f);
    int tail = fgetc(f); /* Exact size, not a prefix of a larger file. */
    int bad = ferror(f);
    fclose(f);
    if (bad || n != sizeof(data) || tail != EOF)
        return -1;
    return spds_decode(record, data, n);
}
int spds_storage_load_profile(const char *base, SpdsRecord *profile) {
    char path[SPDS_PATH_MAX];
    if (path_join(path, sizeof(path), base, "profile.sps")) return -1;
    return read_packet(path, profile);
}
int spds_storage_save_profile(const char *base, const SpdsRecord *profile) {
    char tmp[SPDS_PATH_MAX], final[SPDS_PATH_MAX], outbound[SPDS_PATH_MAX];
    uint8_t data[SPDS_WIRE_SIZE];
    if (spds_encode(profile, data) ||
        path_join(tmp, sizeof(tmp), base, "profile.tmp") ||
        path_join(final, sizeof(final), base, "profile.sps") ||
        path_join(outbound, sizeof(outbound), base, "outbox/profile.sps"))
        return -1;
    FILE *f = fopen(tmp, "wb");
    if (!f) return -1;
    int ok = fwrite(data, 1, sizeof(data), f) == sizeof(data);
    if (fclose(f) != 0) ok = 0;
    if (!ok || rename(tmp, final) != 0) {
        remove(tmp);
        return -1;
    }
    f = fopen(outbound, "wb");
    if (!f) return -1;
    ok = fwrite(data, 1, sizeof(data), f) == sizeof(data);
    if (fclose(f) != 0) ok = 0;
    if (!ok) { remove(outbound); return -1; }
    return 0;
}
static int seen_recent(const char *logpath, uint64_t id, uint32_t now) {
    FILE *f = fopen(logpath, "r");
    if (!f) return errno == ENOENT ? 0 : -1;
    char line[128];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        unsigned long long previous_id = 0;
        unsigned long previous_at = 0;
        if (sscanf(line, "%16llx\t%lu\t", &previous_id, &previous_at) == 2 &&
            (uint64_t)previous_id == id && now >= previous_at &&
            now - previous_at < SPDS_COOLDOWN_SECONDS) {
            found = 1;
            break;
        }
    }
    if (ferror(f)) found = -1;
    fclose(f);
    return found;
}
static int accept(const char *logpath, const SpdsRecord *r, uint32_t now) {
    int seen = seen_recent(logpath, r->sender_id, now);
    if (seen != 0) return seen > 0 ? 0 : -1;
    FILE *f = fopen(logpath, "a");
    if (!f) return -1;
    int result = fprintf(f, "%016llx\t%lu\t%s\n",
             (unsigned long long)r->sender_id, (unsigned long)now, r->name);
    int ok = result > 0 && fflush(f) == 0;
    if (fclose(f) != 0) ok = 0;
    return ok ? 1 : -1;
}
int spds_storage_receive(const char *base, const SpdsRecord *r,
                         uint64_t own_id, uint32_t now) {
    char logpath[SPDS_PATH_MAX];
    if (!r || !base || !r->sender_id || r->sender_id == own_id)
        return r && r->sender_id == own_id ? 0 : -1;
    if (path_join(logpath, sizeof(logpath), base, "encounters.tsv"))
        return -1;
    return accept(logpath, r, now);
}
int spds_storage_poll(const char *base, uint64_t own_id,
                      uint32_t now, SpdsPollStats *stats) {
    char dir[SPDS_PATH_MAX], logpath[SPDS_PATH_MAX], full[SPDS_PATH_MAX];
    if (!stats || path_join(dir, sizeof(dir), base, "inbox") ||
        path_join(logpath, sizeof(logpath), base, "encounters.tsv")) return -1;
    memset(stats, 0, sizeof(*stats));
    DIR *d = opendir(dir);
    if (!d) return -1;
    struct dirent *entry;
    unsigned limit = 0;
    while ((entry = readdir(d)) != NULL && limit < 64) {
        size_t len = strlen(entry->d_name);
        if (len < 5 || strcmp(entry->d_name + len - 4, ".sps") != 0)
            continue;
        if (strchr(entry->d_name, '/') || strchr(entry->d_name, '\\') ||
            path_join(full, sizeof(full), dir, entry->d_name)) {
            ++stats->invalid;
            continue;
        }
        ++limit;
        SpdsRecord rec;
        if (read_packet(full, &rec)) { ++stats->invalid; continue; }
        if (rec.sender_id == own_id) {
            ++stats->duplicates;
            if (remove(full) != 0) ++stats->io_errors;
            continue;
        }
        int result = accept(logpath, &rec, now);
        if (result < 0) { ++stats->io_errors; continue; }
        if (result == 0) ++stats->duplicates;
        else ++stats->accepted;
        if (remove(full) != 0) ++stats->io_errors;
    }
    closedir(d);
    return 0;
}
int spds_storage_total(const char *base, unsigned *total) {
    char path[SPDS_PATH_MAX], line[128];
    if (!total || path_join(path, sizeof(path), base, "encounters.tsv")) return -1;
    *total = 0;
    FILE *f = fopen(path, "r");
    if (!f) return errno == ENOENT ? 0 : -1;
    while (fgets(line, sizeof(line), f)) ++*total;
    int bad = ferror(f);
    fclose(f);
    return bad ? -1 : 0;
}
