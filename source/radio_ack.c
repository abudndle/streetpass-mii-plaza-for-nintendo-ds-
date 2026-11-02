#include "radio_ack.h"
#include "protocol.h"
#include <string.h>
static void put32(uint8_t *p,uint32_t n) {
    for(unsigned i=0;i<4;i++) p[i]=(uint8_t)(n>>(i*8));
}
static uint32_t get32(const uint8_t *p) {
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
static void put64(uint8_t *p,uint64_t n) {
    for(unsigned i=0;i<8;i++) p[i]=(uint8_t)(n>>(i*8));
}
static uint64_t get64(const uint8_t *p) {
    uint64_t n=0;
    for(unsigned i=0;i<8;i++) n|=(uint64_t)p[i]<<(i*8);
    return n;
}
int spds_ack_encode(uint8_t out[SPDS_ACK_SIZE],uint64_t sender,uint32_t nonce) {
    if(!out || !sender) return 0;
    memset(out,0,SPDS_ACK_SIZE);
    memcpy(out,"SPDA",4);
    out[4]=1;
    put64(out+8,sender);
    put32(out+16,nonce);
    put32(out+20,spds_crc32(out,20));
    return 1;
}
int spds_ack_decode(SpdsAck *out,const uint8_t *p,size_t n) {
    if(!out || !p || n!=SPDS_ACK_SIZE || memcmp(p,"SPDA",4) ||
       p[4]!=1 || p[5] || p[6] || p[7] ||
       get32(p+20)!=spds_crc32(p,20) || !get64(p+8)) return 0;
    out->sender_id=get64(p+8);
    out->nonce=get32(p+16);
    return 1;
}
int spds_ack_tracker_track(SpdsAckTracker *s,uint64_t sender,uint32_t nonce) {
    if(!s || !sender) return 0;
    s->pending_sender=sender;
    s->pending_nonce=nonce;
    s->awaiting=1;
    return 1;
}
int spds_ack_tracker_consume(SpdsAckTracker *s,const uint8_t *p,size_t n) {
    if(!s) return 0;
    SpdsAck ack;
    if(!spds_ack_decode(&ack,p,n)) {++s->invalid;return 0;}
    if(!s->awaiting) return 0;
    if(ack.sender_id!=s->pending_sender || ack.nonce!=s->pending_nonce) {
        ++s->unmatched;
        return 0;
    }
    s->awaiting=0;
    ++s->confirmed;
    return 1;
}
