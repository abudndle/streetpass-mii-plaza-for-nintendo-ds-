#include "relay.h"
#include <string.h>
static void put32(uint8_t *p,uint32_t n){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(n>>(8*i));}
static uint32_t get32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
void spds_relay_init(SpdsRelay *r,uint64_t id,int enabled){if(r){memset(r,0,sizeof *r);r->own_id=id;r->enabled=!!enabled;}}
int spds_relay_encode(uint8_t out[SPDS_RELAY_SIZE],const SpdsRecord *src,unsigned hops,uint32_t when){
    if(!out||!src||hops>SPDS_RELAY_MAX_HOPS)return 0;
    uint8_t packet[SPDS_RELAY_SIZE]={0};
    memcpy(packet,"SPDR",4);packet[4]=1;packet[5]=(uint8_t)hops;
    if(spds_encode(src,packet+8)!=0)return 0;
    put32(packet+72,when);put32(packet+76,spds_crc32(packet,76));
    memcpy(out,packet,sizeof packet);return 1;
}
int spds_relay_decode(SpdsRecord *src,unsigned *hops,uint32_t *when,const uint8_t *p,unsigned n){
    if(!src||!hops||!when||!p||n!=SPDS_RELAY_SIZE||memcmp(p,"SPDR",4)||
       p[4]!=1||p[5]>SPDS_RELAY_MAX_HOPS||p[6]||p[7]||
       get32(p+76)!=spds_crc32(p,76))return 0;
    SpdsRecord temp;if(spds_decode(&temp,p+8,SPDS_WIRE_SIZE)!=0)return 0;
    *src=temp;*hops=p[5];*when=get32(p+72);return 1;
}
int spds_relay_accept(SpdsRelay *r,const uint8_t *p,unsigned n,uint32_t now,SpdsRecord *out){
    if(!r||!out)return -1;
    SpdsRecord src;unsigned hops;uint32_t when;
    if(!spds_relay_decode(&src,&hops,&when,p,n)||src.sender_id==0)return -1;
    if(src.sender_id==r->own_id)return 0;
    if(now>=1577836800u&&when>=1577836800u&&
       ((when>now&&when-now>300u)||(now>when&&now-when>120u)))return -1;
    for(unsigned i=0;i<SPDS_RELAY_CACHE;i++){
        SpdsRelaySeen *seen=&r->seen[i];
        if(seen->sender==src.sender_id&&seen->nonce==src.nonce&&
           now>=seen->heard&&now-seen->heard<120u){r->duplicates++;return 0;}
    }
    SpdsRelaySeen *slot=&r->seen[r->next++%SPDS_RELAY_CACHE];
    memset(slot,0,sizeof *slot);slot->sender=src.sender_id;slot->nonce=src.nonce;
    slot->heard=now;slot->hops=hops;memcpy(slot->packet,p,SPDS_RELAY_SIZE);
    r->received++;*out=src;return 1;
}
int spds_relay_forward(const SpdsRelay *r,unsigned i,uint8_t output[SPDS_RELAY_SIZE]){
    if(!r||!r->enabled||!output||i>=SPDS_RELAY_CACHE)return 0;
    const SpdsRelaySeen *s=&r->seen[i];
    if(!s->sender||!s->hops||s->forwarded)return 0;
    SpdsRecord src;unsigned hops;uint32_t when;
    if(!spds_relay_decode(&src,&hops,&when,s->packet,SPDS_RELAY_SIZE))return 0;
    return spds_relay_encode(output,&src,hops-1u,when);
}
int spds_relay_mark_forwarded(SpdsRelay *r,unsigned i){
    if(!r||i>=SPDS_RELAY_CACHE)return 0;
    SpdsRelaySeen *s=&r->seen[i];
    if(!s->sender||!s->hops||s->forwarded)return 0;
    s->forwarded=1;r->forwarded++;return 1;
}
