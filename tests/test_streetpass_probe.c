#include "streetpass_probe.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Reproduces public 3dbrew frame structure with anonymized MAC and consoleID.
 * Nintendo vendor IE length=21; four-byte Wi-Fi FCS is OUTSIDE the IE. */
static uint32_t crc32(const uint8_t *p,size_t n){uint32_t c=UINT32_C(0xffffffff);for(size_t i=0;i<n;i++){c^=p[i];for(unsigned b=0;b<8;b++)c=(c>>1u)^((c&1u)?UINT32_C(0xedb88320):0u);}return ~c;}
static size_t make_frame(uint8_t *p,int multiple){
 const char ssid[]="Nintendo_3DS_continuous_scan_000";
 memset(p,0,256);p[0]=0x40;
 for(unsigned i=4;i<10;i++)p[i]=0xff;
 for(unsigned i=16;i<22;i++)p[i]=0xff;
 size_t n=24;
 p[n++]=0;p[n++]=(uint8_t)(sizeof(ssid)-1u);memcpy(p+n,ssid,sizeof(ssid)-1u);n+=sizeof(ssid)-1u;
 const uint8_t rates[]={1,8,0x82,0x84,0x8b,0x0c,0x12,0x96,0x18,0x24,50,4,0x30,0x48,0x60,0x6c};
 memcpy(p+n,rates,sizeof(rates));n+=sizeof(rates);
 p[n++]=221;p[n++]=(uint8_t)(multiple?26u:21u);
 const uint8_t prefix[]={0,0x1f,0x32,1,0x11};memcpy(p+n,prefix,sizeof(prefix));n+=sizeof(prefix);
 p[n++]=(uint8_t)(multiple?10u:5u);
 if(multiple){const uint8_t other[]={0,3,0x58,0,0x30};memcpy(p+n,other,sizeof(other));n+=sizeof(other);}
 const uint8_t plaza[]={0,2,8,0,0};memcpy(p+n,plaza,sizeof(plaza));n+=sizeof(plaza);
 p[n++]=0xf0;p[n++]=8;memset(p+n,0,8);n+=8;
 return n;
}
static void put_fcs(uint8_t *p,size_t n){uint32_t c=crc32(p,n);for(unsigned j=0;j<4;j++)p[n+j]=(uint8_t)(c>>(j*8u));}
int main(void){
 assert(crc32((const uint8_t *)"123456789",9u)==UINT32_C(0xcbf43926));
 uint8_t frame[256];size_t n=make_frame(frame,0);
 assert(n==97u); /* MAC 24 + SSID 34 + rates 16 + vendor 23 */
 assert(spds_3ds_probe_classify(frame,n)==3u);
 assert(spds_3ds_probe_is_streetpass(frame,n));
 put_fcs(frame,n);assert(spds_3ds_probe_classify(frame,n+4u)==3u);
 uint8_t capture[300]={0};capture[2]=26;capture[4]=0x2f;capture[5]=0x48;capture[16]=0x12;
 memcpy(capture+26,frame,n+4);assert(spds_3ds_probe_classify_radiotap(capture,26+n+4)==3u);
 capture[2]=27;assert(!spds_3ds_probe_classify_radiotap(capture,26+n+4));capture[2]=26;
 capture[0]=1;assert(!spds_3ds_probe_classify_radiotap(capture,26+n+4));capture[0]=0;
 frame[n+3]^=1;assert(!spds_3ds_probe_classify(frame,n+4));frame[n+3]^=1;
 assert(!spds_3ds_probe_classify(frame,n-1));
 assert(!spds_3ds_probe_classify(NULL,n));
 assert(!spds_3ds_probe_classify(frame,SPDS_3DS_PROBE_MAX_FRAME+1u));
 frame[0]=0x50;assert(!spds_3ds_probe_classify(frame,n));frame[0]=0x40;
 /* Plaza service flags are not fixed to zero. */
 const size_t flags=n-8u-2u-1u;
 assert(frame[flags]==0u);frame[flags]=0x20u;
 assert(spds_3ds_probe_classify(frame,n)==3u);frame[flags]=0u;
 frame[flags-4u]=0x01u;
 assert(spds_3ds_probe_classify(frame,n)==1u);frame[flags-4u]=0x00u;
 assert(spds_3ds_probe_classify(frame,n)==3u);
 n=make_frame(frame,1);
 assert(spds_3ds_probe_classify(frame,n)==3u);
 put_fcs(frame,n);assert(spds_3ds_probe_classify(frame,n+4u)==3u);
 assert(!spds_3ds_probe_classify(frame,n-1u));
 /* Changing the Nintendo OUI removes the advertised-service identity. */
 size_t vendor_start=n-26u;frame[vendor_start+2u]^=1u;
 assert(!spds_3ds_probe_classify(frame,n));
 puts("PASS: anonymized 3DS probe frame, FCS/radiotap, multiple services and flags");
 return 0;
}
