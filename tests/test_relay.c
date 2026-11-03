#include "relay.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 SpdsRecord own={12,1700000000,3,"Alice"},decoded;SpdsRelay r;uint8_t packet[SPDS_RELAY_SIZE],next[SPDS_RELAY_SIZE];unsigned hops;uint32_t at;
 spds_relay_init(&r,24,1);assert(spds_relay_encode(packet,&own,2,1700000000));assert(spds_relay_accept(&r,packet,sizeof packet,1700000001,&decoded)==1);assert(decoded.sender_id==12&&r.received==1);
 assert(!spds_relay_forward(&r,0,next));assert(spds_relay_accept(&r,packet,sizeof packet,1700000002,&decoded)==0);assert(spds_relay_mark_durable(&r,own.sender_id,own.nonce));
 assert(spds_relay_forward(&r,0,next));assert(r.forwarded==1);assert(!spds_relay_forward(&r,0,next));assert(spds_relay_decode(&decoded,&hops,&at,next,sizeof next));assert(hops==1&&at==1700000000);
 SpdsRelay second;spds_relay_init(&second,44,1);assert(spds_relay_accept(&second,next,sizeof next,1700000002,&decoded)==1);assert(!spds_relay_forward(&second,0,next));assert(spds_relay_mark_durable(&second,own.sender_id,own.nonce));assert(spds_relay_forward(&second,0,next));assert(spds_relay_decode(&decoded,&hops,&at,next,sizeof next)&&hops==0);assert(!spds_relay_forward(&second,0,next));
 SpdsRelay third;spds_relay_init(&third,88,1);assert(spds_relay_accept(&third,next,sizeof next,1700000003,&decoded)==1);assert(!spds_relay_forward(&third,0,next));
 SpdsRelay chain[5];uint8_t transit[SPDS_RELAY_SIZE],forwarded[SPDS_RELAY_SIZE];assert(spds_relay_encode(transit,&own,SPDS_RELAY_MAX_HOPS,1700000000));
 for(unsigned i=0;i<5;i++){spds_relay_init(&chain[i],100u+i,1);assert(spds_relay_accept(&chain[i],transit,sizeof transit,1700000001+i,&decoded)==1);assert(decoded.sender_id==own.sender_id&&decoded.nonce==own.nonce);assert(!strcmp(decoded.name,own.name));assert(spds_relay_accept(&chain[i],transit,sizeof transit,1700000001+i,&decoded)==0);assert(!spds_relay_forward(&chain[i],0,forwarded));assert(spds_relay_mark_durable(&chain[i],own.sender_id,own.nonce));if(i<4){assert(spds_relay_forward(&chain[i],0,forwarded));assert(!spds_relay_forward(&chain[i],0,forwarded));assert(spds_relay_decode(&decoded,&hops,&at,forwarded,sizeof forwarded));assert(hops==SPDS_RELAY_MAX_HOPS-i-1);memcpy(transit,forwarded,sizeof transit);}else assert(!spds_relay_forward(&chain[i],0,forwarded));}
 assert(!spds_relay_encode(packet,&own,SPDS_RELAY_MAX_HOPS+1,1700000000));SpdsRecord zero={0,1700000000,19,"Invalid"};assert(!spds_relay_encode(packet,&zero,2,1700000000));assert(spds_relay_encode(packet,&own,2,1700000000));assert(spds_relay_accept(&r,packet,sizeof packet,0xffffffffu,&decoded)==-1);packet[13]^=1;assert(spds_relay_accept(&r,packet,sizeof packet,1700000004,&decoded)==-1);assert(!spds_relay_decode(&decoded,&hops,&at,packet,sizeof packet-1));spds_relay_init(&r,24,0);assert(spds_relay_accept(&r,next,sizeof next,1700000003,&decoded)==1);assert(!spds_relay_forward(&r,0,packet));
 /* A failed physical TX cannot consume a cached relay record. */
 SpdsRelay retry;spds_relay_init(&retry,555,1);
 assert(spds_relay_encode(packet,&own,2,1700000000));
 assert(spds_relay_accept(&retry,packet,sizeof packet,1700000006,&decoded)==1);
 assert(spds_relay_mark_durable(&retry,own.sender_id,own.nonce));
 assert(spds_relay_prepare(&retry,0,forwarded));
 assert(retry.forwarded==0 && !retry.seen[0].forwarded);
 assert(spds_relay_prepare(&retry,0,forwarded)); /* retry after failed TX */
 assert(spds_relay_commit_forward(&retry,0));
 assert(retry.forwarded==1 && retry.seen[0].forwarded);
 assert(!spds_relay_prepare(&retry,0,forwarded));
 assert(!spds_relay_commit_forward(&retry,0));
 puts("PASS: relay durable-before-forward, retryable TX, duplicate, four-hop cap and opt-in");return 0;
}
