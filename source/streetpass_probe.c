#include "streetpass_probe.h"
#include <string.h>
static const uint8_t ssid[]="Nintendo_3DS_continuous_scan_000";
static const uint8_t plaza_prefix[]={0x00,0x02,0x08,0x00};
static uint32_t fcs32(const uint8_t *p,size_t n){
 uint32_t crc=UINT32_C(0xffffffff);
 for(size_t i=0;i<n;i++){crc^=p[i];for(unsigned b=0;b<8;b++)crc=(crc>>1u)^((crc&1u)?UINT32_C(0xedb88320):0u);}
 return ~crc;
}
/* Service IDs are five bytes; the final byte is a send-mode flag.
 * Some services carry an additional six bytes of 0xff. */
static int has_plaza(const uint8_t *p,size_t n){
 size_t i=0;int found=0;
 /* Reject truncated service entries, including a trailing partial entry.
  * A service is five bytes, optionally followed by six 0xff bytes. */
 while(i<n){
  if(n-i<5u)return 0;
  if(!memcmp(p+i,plaza_prefix,sizeof(plaza_prefix)))found=1;
  i+=5u;
  if(n-i>=6u){
   int all_ff=1;
   for(unsigned j=0;j<6u;j++)if(p[i+j]!=0xffu)all_ff=0;
   if(all_ff)i+=6u;
  }
 }
 return found;
}
static unsigned classify_80211(const uint8_t *p,size_t n){
 if(!p||n<24u||n>SPDS_3DS_PROBE_MAX_FRAME||p[0]!=0x40u||(p[1]&3u))return 0;
 size_t i=24u;int matching_ssid=0,vendor=0,plaza=0;
 while(i+2u<=n){
  uint8_t tag=p[i],size=p[i+1u];i+=2u;
  if((size_t)size>n-i)return 0;
  if(tag==0u&&size==sizeof(ssid)-1u&&!memcmp(p+i,ssid,sizeof(ssid)-1u))matching_ssid=1;
  if(tag==221u&&size>=4u&&p[i]==0x00u&&p[i+1u]==0x1fu&&p[i+2u]==0x32u&&p[i+3u]==0x01u){
   size_t j=i+4u,end=i+(size_t)size;int valid=1,local_plaza=0;
   while(j+2u<=end){uint8_t type=p[j],count=p[j+1u];j+=2u;
    if((size_t)count>end-j){valid=0;break;}
    if(type==0x11u&&has_plaza(p+j,count))local_plaza=1;
    j+=count;
   }
   if(j!=end)valid=0;
   if(valid){vendor=1;if(local_plaza)plaza=1;}
  }
  i+=size;
 }
 if(i!=n||!matching_ssid||!vendor)return 0;
 return SPDS_3DS_PROBE_STREETPASS|(plaza?SPDS_3DS_PROBE_PLAZA:0u);
}
unsigned spds_3ds_probe_classify(const uint8_t *p,size_t n){
 if(!p||n>SPDS_3DS_PROBE_MAX_FRAME)return 0;
 unsigned result=classify_80211(p,n);
 if(result||n<28u)return result;
 size_t payload=n-4u;
 uint32_t fcs=(uint32_t)p[payload]|((uint32_t)p[payload+1u]<<8u)|((uint32_t)p[payload+2u]<<16u)|((uint32_t)p[payload+3u]<<24u);
 if(fcs!=fcs32(p,payload))return 0;
 return classify_80211(p,payload);
}
int spds_3ds_probe_is_streetpass(const uint8_t *p,size_t n){return (spds_3ds_probe_classify(p,n)&SPDS_3DS_PROBE_STREETPASS)!=0u;}
unsigned spds_3ds_probe_classify_radiotap(const uint8_t *p,size_t n){
 if(!p||n<8u||p[0]!=0u||p[1]!=0u)return 0;
 size_t hdr=(size_t)p[2]|((size_t)p[3]<<8u);
 if(hdr<8u||hdr>=n||n-hdr>SPDS_3DS_PROBE_MAX_FRAME)return 0;
 return spds_3ds_probe_classify(p+hdr,n-hdr);
}
