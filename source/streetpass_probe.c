#include "streetpass_probe.h"
#include <string.h>

static const uint8_t streetpass_ssid[]="Nintendo_3DS_continuous_scan_000";
/* 3dbrew records this five-byte Mii Plaza service code inside Nintendo's
 * vendor information element. The bytes can be spoofed; this is passive
 * recognition only, not a CECD handshake or proof of the sender. */
static const uint8_t plaza_service[]={0x00,0x02,0x08,0x00,0x00};

unsigned spds_3ds_probe_classify(const uint8_t *frame,size_t length) {
    if(!frame || length<24u || length>SPDS_3DS_PROBE_MAX_FRAME ||
       (frame[0]&0xfcu)!=0x40u || (frame[1]&0x03u)!=0u) return 0;
    size_t pos=24u;
    int ssid=0,vendor=0,plaza=0;
    while(pos+2u<=length) {
        uint8_t id=frame[pos], size=frame[pos+1u];
        pos+=2u;
        if((size_t)size>length-pos) return 0;
        if(id==0u && size==sizeof(streetpass_ssid)-1u &&
           !memcmp(frame+pos,streetpass_ssid,sizeof(streetpass_ssid)-1u))
            ssid=1;
        if(id==221u && size>=4u &&
           frame[pos]==0x00u && frame[pos+1u]==0x1fu &&
           frame[pos+2u]==0x32u && frame[pos+3u]==0x01u) {
            vendor=1;
            /* Length-checked Nintendo service TLVs after OUI and type. */
            size_t off=pos+4u,end=pos+(size_t)size;
            while(off+2u<=end) {
                unsigned tag=frame[off],n=frame[off+1u];
                off+=2u;
                if((size_t)n>end-off) break;
                if(tag==0x11u && n>=sizeof(plaza_service) &&
                   !memcmp(frame+off,plaza_service,sizeof(plaza_service)))
                    plaza=1;
                off+=(size_t)n;
            }
        }
        pos+=(size_t)size;
    }
    if(pos!=length || !ssid || !vendor) return 0;
    return SPDS_3DS_PROBE_STREETPASS |
           (plaza ? SPDS_3DS_PROBE_PLAZA : 0u);
}
int spds_3ds_probe_is_streetpass(const uint8_t *frame,size_t length) {
    return (spds_3ds_probe_classify(frame,length) &
            SPDS_3DS_PROBE_STREETPASS)!=0u;
}
