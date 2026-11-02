#ifndef SPDS_VISITOR_H
#define SPDS_VISITOR_H
#include "mii_bridge.h"
#include "protocol.h"
#include "storage.h"

/* SPDV v1: magic+version+reserved (8), SPDS envelope (64),
 * canonical MDS3 full appearance (134), CRC32(206) (4). */
#define SPDS_VISITOR_SIZE 210u
typedef struct {
    unsigned accepted;
    unsigned duplicates;
    unsigned invalid;
    unsigned io_errors;
} SpdsVisitorStats;

/* Only shareable Mii profiles are valid for an outbound bundle. */
int spds_visitor_encode(uint8_t out[SPDS_VISITOR_SIZE],
                        const SpdsRecord *record,const SpdsMii *mii);
int spds_visitor_decode(SpdsRecord *record,SpdsMii *mii,
                        const uint8_t *data,size_t n);
int spds_visitor_export(const char *base,const SpdsRecord *profile,
                        const SpdsMii *mii);
int spds_visitor_poll(const char *base,uint64_t own_id,
                      uint32_t now,SpdsVisitorStats *stats);
/* Read cached appearance for a specific sender ID, never alter source data. */
int spds_visitor_read(const char *base,uint64_t sender_id,SpdsMii *out);
#endif
