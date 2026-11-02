#ifndef SPDS_MII_BRIDGE_H
#define SPDS_MII_BRIDGE_H
#include <stddef.h>
#include <stdint.h>

/* Mii Maker DS independently specified MDS2/MDS3 134-byte avatar packets,
 * compact MDSC 8-KiB and legacy MDSV 64-KiB save snapshots.
 * Source files are strictly read-only and never mutated by Plaza.
 */
#define SPDS_MII_PACKET 134u
#define SPDS_MII_FIELDS 62u
#define SPDS_MII_NAME 32u

typedef struct {
    char name[SPDS_MII_NAME];
    char creator[SPDS_MII_NAME];
    uint8_t features[SPDS_MII_FIELDS];
    uint8_t packet[SPDS_MII_PACKET]; /* Canonical MDS3 cached packet */
    unsigned slot;                  /* original gallery slot, or 0 for standalone */
    int from_gallery;
} SpdsMii;

/* 1 on a decoded valid Mii, 0 for corrupted or unsupported data. */
int spds_mii_decode_packet(SpdsMii *out, const uint8_t *bytes, size_t length);
int spds_mii_from_image(SpdsMii *out, const uint8_t *bytes, size_t length);
/* Read an explicit file supplied by the user, never search arbitrary saves. */
int spds_mii_from_file(SpdsMii *out, const char *path);
int spds_mii_cache_read(SpdsMii *out, const char *service_dir);
int spds_mii_cache_write(const SpdsMii *mii, const char *service_dir);
/* Produce 7-bit safe nickname for current SPDS v1 exchange. */
void spds_mii_radio_name(char out[SPDS_MII_NAME], const SpdsMii *mii);
#endif
