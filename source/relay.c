#include "relay.h"
#include <string.h>
static void put32(uint8_t *p,uint32_t n){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(n>>(8*i));}
static uint32_t get32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
void spds_relay_init(SpdsRelay *r,uint64_t id,int enabled){if(r){memset(r,0,sizeof *r);r->own_id=id;r->enabled=!!enabled;}}
int spds_relay_encode(uint8_t out[SPDS_RELAY_SIZE],const SpdsRecord *src,unsigned hops,uint32_t when){
 if(!out||!src||!src->sender_id||hops>SPDS_RELAY_MAX_HOPS)return 0;
 uint8_t p[SPDS_RELAY_SIZE]={0};memcpy(p,"SPDR",4);p[4]=1;p[5]=(uint8_t)hops;
 if(spds_encode(src,p+8)!=0)return 0;
 put32(p+72,when);put32(p+76,spds_crc32(p,76));memcpy(out,p,sizeof p);return 1;
}
int spds_relay_decode(SpdsRecord *src,unsigned *hops,uint32_t *when,const uint8_t *p,unsigned n){
 if(!src||!hops||!when||!p||n!=SPDS_RELAY_SIZE||memcmp(p,"SPDR",4)||p[4]!=1||p[5]>SPDS_RELAY_MAX_HOPS||p[6]||p[7]||get32(p+76)!=spds_crc32(p,76))return 0;
 SpdsRecord t;if(spds_decode(&t,p+8,SPDS_WIRE_SIZE)!=0)return 0;*src=t;*hops=p[5];*when=get32(p+72);return 1;
}
int spds_relay_accept(SpdsRelay *r,const uint8_t *p,unsigned n,uint32_t now,SpdsRecord *out){
 if(!r||!out)return -1;
 SpdsRecord src;unsigned hops;uint32_t when;
 if(!spds_relay_decode(&src,&hops,&when,p,n)||!src.sender_id)return -1;
 if(src.sender_id==r->own_id)return 0;
 if(now>=1577836800u&&when>=1577836800u&&((when>now&&when-now>300u)||(now>when&&now-when>120u)))return -1;
 for(unsigned i=0;i<SPDS_RELAY_CACHE;i++){SpdsRelaySeen *s=&r->seen[i];if(s->sender==src.sender_id&&s->nonce==src.nonce&&now>=s->heard&&now-s->heard<120u){r->duplicates++;return 0;}}
 SpdsRelaySeen *s=&r->seen[r->next++%SPDS_RELAY_CACHE];memset(s,0,sizeof *s);s->sender=src.sender_id;s->nonce=src.nonce;s->heard=now;s->hops=hops;memcpy(s->packet,p,SPDS_RELAY_SIZE);r->received++;*out=src;return 1;
}
int spds_relay_mark_durable(SpdsRelay *r,uint64_t sender,uint32_t nonce){
 if(!r||!sender)return 0;
 for(unsigned i=0;i<SPDS_RELAY_CACHE;i++){SpdsRelaySeen *s=&r->seen[i];if(s->sender==sender&&s->nonce==nonce){s->durable=1;return 1;}}
 return 0;
}
int spds_relay_prepare(SpdsRelay *r,unsigned i,uint8_t out[SPDS_RELAY_SIZE]){
 if(!r||!r->enabled||!out||i>=SPDS_RELAY_CACHE)return 0;
 SpdsRelaySeen *s=&r->seen[i];if(!s->sender||!s->durable||!s->hops||s->forwarded)return 0;
 SpdsRecord src;unsigned hops;uint32_t when;
 if(!spds_relay_decode(&src,&hops,&when,s->packet,SPDS_RELAY_SIZE)||!spds_relay_encode(out,&src,hops-1u,when))return 0;
 return 1;
}
int spds_relay_commit_forward(SpdsRelay *r,unsigned i) {
 if(!r||!r->enabled||i>=SPDS_RELAY_CACHE)return 0;
 SpdsRelaySeen *s=&r->seen[i];
 if(!s->sender||!s->durable||!s->hops||s->forwarded)return 0;
 s->forwarded=1;r->forwarded++;return 1;
}
/* Legacy combined operation retained for existing callers/tests. */
int spds_relay_forward(SpdsRelay *r,unsigned i,uint8_t out[SPDS_RELAY_SIZE]){
 return spds_relay_prepare(r,i,out) && spds_relay_commit_forward(r,i);
}
