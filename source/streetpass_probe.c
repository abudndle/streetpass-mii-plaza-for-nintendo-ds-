#include "streetpass_probe.h"
#include <string.h>
static const uint8_t streetpass_ssid[]="Nintendo_3DS_continuous_scan_000";
int spds_3ds_probe_is_streetpass(const uint8_t *frame,size_t length) {
    /* 802.11 management probe request: FC type=0/subtype=4;
     * 24-byte header followed by bounded information elements. */
    if(!frame || length<24u || length>SPDS_3DS_PROBE_MAX_FRAME ||
       (frame[0]&0xfcu)!=0x40u || (frame[1]&0x03u)!=0u) return 0;
    size_t pos=24u;
    int ssid=0,oui=0;
    while(pos+2u<=length) {
        uint8_t id=frame[pos], size=frame[pos+1u];
        pos+=2u;
        if((size_t)size>length-pos) return 0;
        if(id==0u && size==sizeof(streetpass_ssid)-1u &&
           memcmp(frame+pos,streetpass_ssid,sizeof(streetpass_ssid)-1u)==0)
            ssid=1;
        if(id==221u && size>=3u &&
           frame[pos]==0x00u && frame[pos+1u]==0x1fu && frame[pos+2u]==0x32u)
            oui=1;
        pos+=(size_t)size;
    }
    return pos==length && ssid && oui;
}
