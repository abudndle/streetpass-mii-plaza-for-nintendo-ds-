#include "visitor.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void put32(uint8_t *p,uint32_t n) {
    for (unsigned k=0;k<4;k++) p[k]=(uint8_t)(n>>(8*k));
}
static void make_mii(SpdsMii *mii,const char *name,int sharing) {
    uint8_t packet[134]={0};
    memcpy(packet,"MDS3",4);
    strcpy((char *)packet+4,name);
    strcpy((char *)packet+36,"Creator");
    packet[68+47]=(uint8_t)sharing;
    packet[68+51]=1;
    put32(packet+130,spds_crc32(packet,130));
    assert(spds_mii_decode_packet(mii,packet,134));
}
int main(void) {
    SpdsMii mii,visitor;
    make_mii(&mii,"Real DS Mii",1);
    SpdsRecord owner={345,1700000000u,99,"Real DS Mii"};
    uint8_t wire[SPDS_VISITOR_SIZE];
    assert(spds_visitor_encode(wire,&owner,&mii));
    SpdsRecord decoded;
    assert(spds_visitor_decode(&decoded,&visitor,wire,sizeof wire));
    assert(!strcmp(visitor.name,"Real DS Mii") && decoded.sender_id==345);
    wire[80]^=1;
    assert(!spds_visitor_decode(&decoded,&visitor,wire,sizeof wire));
    wire[80]^=1;
    assert(!spds_visitor_decode(&decoded,&visitor,wire,sizeof wire-1));
    make_mii(&mii,"Private Mii",0);
    strcpy(owner.name,"Private Mii");
    assert(!spds_visitor_encode(wire,&owner,&mii));
    make_mii(&mii,"Real DS Mii",1);
    strcpy(owner.name,"Real DS Mii");
    char base[]="/tmp/spds-visitor-XXXXXX";
    assert(mkdtemp(base));
    assert(spds_storage_init(base)==0);
    assert(spds_visitor_export(base,&owner,&mii));
    char outgoing[300],incoming[300];
    snprintf(outgoing,sizeof outgoing,"%s/outbox/visitor.spv",base);
    snprintf(incoming,sizeof incoming,"%s/inbox/visitor.spv",base);
    FILE *f=fopen(outgoing,"rb");assert(f);
    assert(fread(wire,1,sizeof wire,f)==sizeof wire);assert(fclose(f)==0);
    f=fopen(incoming,"wb");assert(f);
    assert(fwrite(wire,1,sizeof wire,f)==sizeof wire);assert(fclose(f)==0);
    SpdsVisitorStats stats;
    assert(spds_visitor_poll(base,111,1700000000u,&stats)==0);
    assert(stats.accepted==1);
    assert(spds_visitor_read(base,345,&visitor));
    assert(!strcmp(visitor.name,"Real DS Mii"));
    assert(visitor.features[51]==0 && visitor.features[61]==2);
    /* A duplicate pass does not destroy the visitor's avatar. */
    f=fopen(incoming,"wb");assert(f);
    assert(fwrite(wire,1,sizeof wire,f)==sizeof wire);assert(fclose(f)==0);
    assert(spds_visitor_poll(base,111,1700000001u,&stats)==0);
    assert(stats.duplicates==1 && stats.accepted==0);
    puts("PASS: full-Mii visitor bundles, validated import, ownership, sharing and duplicate avoidance");
    return 0;
}
