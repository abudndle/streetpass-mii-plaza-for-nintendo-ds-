#include "streetpass_probe.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Synthetic anonymized MAC header and public StreetPass IE pattern from
 * 3dbrew's documented radio capture. No real console identifiers are used. */
static size_t make_frame(uint8_t *frame) {
    static const char ssid[]="Nintendo_3DS_continuous_scan_000";
    memset(frame,0,128);
    frame[0]=0x40;
    for(unsigned i=4;i<10;i++) frame[i]=0xff; /* broadcast destination */
    size_t p=24;
    frame[p++]=0;frame[p++]=(uint8_t)(sizeof(ssid)-1u);
    memcpy(frame+p,ssid,sizeof(ssid)-1u);p+=sizeof(ssid)-1u;
    frame[p++]=1;frame[p++]=8;
    static const uint8_t rates[]={0x82,0x84,0x8b,0x0c,0x12,0x96,0x18,0x24};
    memcpy(frame+p,rates,sizeof(rates));p+=sizeof(rates);
    frame[p++]=221;frame[p++]=22;
    const uint8_t tag[]={0x00,0x1f,0x32,0x01,0x11,0x05,
                         0x00,0x02,0x08,0x00,0x00,
                         0xf0,0x08,0xc8,0x34,0x6e,0x05,0x0f,0xc9,0xc6,0x80,0x5b};
    memcpy(frame+p,tag,sizeof(tag));p+=sizeof(tag);
    return p;
}
int main(void) {
    uint8_t f[128];size_t n=make_frame(f);
    assert(spds_3ds_probe_classify(f,n)==(SPDS_3DS_PROBE_STREETPASS|SPDS_3DS_PROBE_PLAZA));
    assert(spds_3ds_probe_is_streetpass(f,n));
    assert(!spds_3ds_probe_classify(f,23));
    assert(!spds_3ds_probe_classify(NULL,n));
    assert(!spds_3ds_probe_classify(f,n-1)); /* vendor IE truncated */
    f[0]=0x50;assert(!spds_3ds_probe_classify(f,n));f[0]=0x40;
    f[1]=0x01;assert(!spds_3ds_probe_classify(f,n));f[1]=0;
    f[26]='X';assert(!spds_3ds_probe_classify(f,n));f[26]='N';
    /* A valid generic StreetPass service is not proof of Mii Plaza. */
    f[n-22u+6u]^=0x01;
    assert(spds_3ds_probe_classify(f,n)==SPDS_3DS_PROBE_STREETPASS);
    f[n-22u+6u]^=0x01;
    /* Wrong Nintendo vendor type must not match. */
    f[n-22u+3u]=0x02;assert(!spds_3ds_probe_classify(f,n));f[n-22u+3u]=0x01;
    assert(spds_3ds_probe_classify(f,n)==3u);
    assert(!spds_3ds_probe_classify(f,SPDS_3DS_PROBE_MAX_FRAME+1u));
    puts("PASS: anonymous 3DS probe and Mii Plaza service recognition; reject truncated/spoof-pattern errors");
    return 0;
}
