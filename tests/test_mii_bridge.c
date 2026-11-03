#include "mii_bridge.h"
#include "service_config.h"
#include "protocol.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void put32(uint8_t *p,uint32_t v) {
    for(unsigned i=0;i<4;i++) p[i]=(uint8_t)(v>>(i*8));
}
static unsigned width(unsigned n) { unsigned w=0; do{++w;n>>=1;}while(n); return w; }
static const uint8_t maxvalues[62]={
 1,5,11,11,11,131,7,1,59,5,7,6,7,12,18,23,7,8,6,11,12,18,
 17,8,18,35,4,8,6,18,8,5,7,20,5,3,7,8,16,1,8,16,30,127,127,
 11,1,1,1,12,31,1,95,95,95,95,95,95,95,95,95,3
};
static void create_packet(uint8_t p[SPDS_MII_PACKET],const char *name,int owner) {
    memset(p,0,SPDS_MII_PACKET);
    memcpy(p,"MDS3",4);
    strcpy((char *)p+4,name);
    strcpy((char *)p+36,"Maker");
    p[68+45]=3;
    p[68+51]=(uint8_t)owner;
    put32(p+130,spds_crc32(p,130));
}
static void compact_pack(uint8_t *record,const uint8_t *packet) {
    memset(record,0,75);
    record[0]=1;
    unsigned bit=0;
    for (unsigned f=0;f<62;f++) {
        unsigned v=packet[68+f],w=width(maxvalues[f]);
        for(unsigned j=0;j<w;j++,bit++) {
            record[1+bit/8]|=((v>>j)&1u)<<(bit%8);
        }
    }
    for (unsigned i=0;i<10;i++) {
        unsigned char c=(unsigned char)packet[4+i];
        record[35+2*i]=c;
        c=(unsigned char)packet[36+i];
        record[55+2*i]=c;
    }
}
static void write_file(const char *path,const void *data,size_t len) {
    FILE *f=fopen(path,"wb"); assert(f);
    assert(fwrite(data,1,len,f)==len); assert(fclose(f)==0);
}
int main(void) {
    uint8_t p[SPDS_MII_PACKET];
    create_packet(p,"From Maker",1);
    SpdsMii mii;
    assert(spds_mii_decode_packet(&mii,p,sizeof p));
    assert(!strcmp(mii.name,"From Maker") && mii.features[51]==1);
    p[90]^=4;
    assert(!spds_mii_decode_packet(&mii,p,sizeof p));
    p[90]^=4;
    assert(!spds_mii_decode_packet(&mii,p,sizeof p-1));

    uint8_t *compact=calloc(1,8192);assert(compact);
    memcpy(compact,"MDSC",4);
    put32(compact+4,2);
    put32(compact+12,7524);
    create_packet(p,"First",0);
    compact_pack(compact+24,p);
    create_packet(p,"My Owner",1);
    compact_pack(compact+24+75*9,p);
    put32(compact+7524,spds_crc32(compact,7524));
    assert(spds_mii_from_image(&mii,compact,8192));
    assert(!strcmp(mii.name,"My Owner") && mii.slot==9 && mii.from_gallery);
    assert(spds_mii_from_image_slot(&mii,compact,8192,0));
    assert(!strcmp(mii.name,"First") && mii.slot==0);
    assert(spds_mii_from_image_slot(&mii,compact,8192,9));
    assert(!strcmp(mii.name,"My Owner") && mii.slot==9);
    assert(!spds_mii_from_image_slot(&mii,compact,8192,8));
    assert(!spds_mii_from_image_slot(&mii,compact,8192,100));
    compact[39]^=1;
    assert(!spds_mii_from_image(&mii,compact,8192));
    compact[39]^=1;
    free(compact);

    uint8_t *legacy=calloc(1,65536);assert(legacy);
    uint8_t *b0=legacy,*b1=legacy+32768;
    memcpy(b0,"MDSV",4);put32(b0+4,1);put32(b0+8,2);put32(b0+12,13524);
    create_packet(p,"Earlier",1);b0[24]=1;memcpy(b0+25,p,134);
    put32(b0+13524,spds_crc32(b0,13524));
    memcpy(b1,"MDSV",4);put32(b1+4,1);put32(b1+8,3);put32(b1+12,13524);
    create_packet(p,"Latest",1);b1[24]=1;memcpy(b1+25,p,134);
    put32(b1+13524,spds_crc32(b1,13524));
    assert(spds_mii_from_image(&mii,legacy,65536));
    assert(!strcmp(mii.name,"Latest"));
    assert(spds_mii_from_image_slot(&mii,legacy,65536,0));
    assert(!strcmp(mii.name,"Latest"));
    assert(!spds_mii_from_image_slot(&mii,legacy,65536,1));
    b1[38]^=1;
    assert(spds_mii_from_image(&mii,legacy,65536));
    assert(!strcmp(mii.name,"Earlier"));
    assert(spds_mii_from_image_slot(&mii,legacy,65536,0));
    assert(!strcmp(mii.name,"Earlier"));
    free(legacy);

    char base[]="/tmp/spds-mii-XXXXXX";assert(mkdtemp(base));
    assert(spds_service_create(base));
    SpdsServiceConfig cfg;
    assert(spds_service_read(base,&cfg) && cfg.enabled && !cfg.radio_enabled);
    char save[300],config[300];
    snprintf(save,sizeof save,"%s/mii-ds.mds",base);
    snprintf(config,sizeof config,"%s/service.cfg",base);
    create_packet(p,"My actual Mii",1);write_file(save,p,sizeof p);
    const char settings[]="enabled=1\nradio_enabled=0\nmii_source=mii-ds.mds\n";
    write_file(config,settings,sizeof settings-1);
    assert(spds_service_read(base,&cfg));
    assert(spds_service_load_mii(base,&cfg,&mii)==1);
    assert(!strcmp(mii.name,"My actual Mii"));
    char nickname[SPDS_MII_NAME];
    spds_mii_radio_name(nickname,&mii);
    assert(!strcmp(nickname,"My actual Mii"));
    /* Cached Mii survives missing original save. */
    assert(unlink(save)==0);
    assert(spds_service_load_mii(base,&cfg,&mii)==1);
    assert(!strcmp(mii.name,"My actual Mii"));
    assert(spds_mii_cache_read(&mii,base));
    puts("PASS: Mii Maker DS packet, compact/legacy gallery, owner selection, CRC, read-only save import, cache");
    return 0;
}
