#include "streetpass_probe.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    static const char ssid[]="Nintendo_3DS_continuous_scan_000";
    uint8_t frame[128]={0};
    frame[0]=0x40;
    size_t pos=24;
    frame[pos++]=0;frame[pos++]=(uint8_t)(sizeof(ssid)-1u);
    memcpy(frame+pos,ssid,sizeof(ssid)-1u);pos+=sizeof(ssid)-1u;
    frame[pos++]=221;frame[pos++]=5;
    frame[pos++]=0;frame[pos++]=0x1f;frame[pos++]=0x32;
    frame[pos++]=0x01;frame[pos++]=0x02;
    assert(spds_3ds_probe_is_streetpass(frame,pos));
    assert(!spds_3ds_probe_is_streetpass(frame,pos-1));
    frame[0]=0x50;assert(!spds_3ds_probe_is_streetpass(frame,pos));
    frame[0]=0x40;
    frame[24+2+sizeof(ssid)-2]='X';assert(!spds_3ds_probe_is_streetpass(frame,pos));
    frame[24+2+sizeof(ssid)-2]='0';
    frame[pos-3]=0xff;assert(!spds_3ds_probe_is_streetpass(frame,pos));
    frame[pos-3]=0x32;
    assert(spds_3ds_probe_is_streetpass(frame,pos));
    frame[25]=120;assert(!spds_3ds_probe_is_streetpass(frame,pos));
    assert(!spds_3ds_probe_is_streetpass(frame,23));
    assert(!spds_3ds_probe_is_streetpass(NULL,pos));
    puts("PASS: 3DS StreetPass passive probe classifier, truncation and false positives");
    return 0;
}
