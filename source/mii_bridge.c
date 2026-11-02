#include "mii_bridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMPACT_BYTES 8192u
#define LEGACY_BYTES 65536u
#define SLOTS 100u
#define COMPACT_RECORD 75u
#define COMPACT_BODY (24u + COMPACT_RECORD*SLOTS)
#define LEGACY_RECORD (SPDS_MII_PACKET+1u)
#define LEGACY_BODY (24u + LEGACY_RECORD*SLOTS)
#define LEGACY_BANK 32768u

static const uint8_t max_value[SPDS_MII_FIELDS] = {
  1,5,11,11,11,131,7,1,59,5,7,6,7,12,18,23,7,8,6,11,12,18,
  17,8,18,35,4,8,6,18,8,5,7,20,5,3,7,8,16,1,8,16,30,127,127,
  11,1,1,1,12,31,1,95,95,95,95,95,95,95,95,95,3
};
static uint32_t get32(const uint8_t *p) {
    return (uint32_t)p[0] | (uint32_t)p[1]<<8 |
           (uint32_t)p[2]<<16 | (uint32_t)p[3]<<24;
}
static void put32(uint8_t *p, uint32_t v) {
    for (unsigned i=0;i<4;i++) p[i]=(uint8_t)(v>>(8*i));
}
static uint32_t crc(const uint8_t *p, size_t n) {
    uint32_t c=~(uint32_t)0;
    for (size_t i=0;i<n;i++) {
        c^=p[i];
        for (unsigned j=0;j<8;j++)
            c=(c>>1)^((c&1u) ? UINT32_C(0xedb88320) : 0);
    }
    return ~c;
}
static int names_terminated(const char *n) {
    return memchr(n, 0, SPDS_MII_NAME)!=NULL;
}
static int valid(const SpdsMii *mii) {
    if (!names_terminated(mii->name) || !mii->name[0] ||
        !names_terminated(mii->creator)) return 0;
    for (unsigned i=0;i<SPDS_MII_FIELDS;i++)
        if (mii->features[i]>max_value[i]) return 0;
    return 1;
}
static void packet_from_mii(SpdsMii *mii) {
    memset(mii->packet,0,SPDS_MII_PACKET);
    memcpy(mii->packet,"MDS3",4);
    memcpy(mii->packet+4,mii->name,SPDS_MII_NAME);
    memcpy(mii->packet+36,mii->creator,SPDS_MII_NAME);
    memcpy(mii->packet+68,mii->features,SPDS_MII_FIELDS);
    put32(mii->packet+130,crc(mii->packet,130));
}
int spds_mii_decode_packet(SpdsMii *out,const uint8_t *p,size_t len) {
    if (!out || !p || len != SPDS_MII_PACKET ||
       (memcmp(p,"MDS2",4) && memcmp(p,"MDS3",4)) ||
        get32(p+130)!=crc(p,130)) return 0;
    SpdsMii tmp;
    memset(&tmp,0,sizeof tmp);
    memcpy(tmp.name,p+4,SPDS_MII_NAME);
    memcpy(tmp.creator,p+36,SPDS_MII_NAME);
    memcpy(tmp.features,p+68,SPDS_MII_FIELDS);
    if (!memcmp(p,"MDS2",4)) {
        tmp.features[61]=0;
        if (tmp.features[51]) tmp.features[46]=1;
    }
    if (!valid(&tmp)) return 0;
    packet_from_mii(&tmp);
    *out=tmp;
    return 1;
}
/* Decode the original Mii Maker DS 10-code-unit UTF16LE name format. */
static int from_utf16(char out[SPDS_MII_NAME],const uint8_t *data) {
    unsigned pos=0;
    memset(out,0,SPDS_MII_NAME);
    for (unsigned i=0;i<10;i++) {
        uint32_t cp=(uint32_t)data[i*2] | (uint32_t)data[i*2+1]<<8;
        if (!cp) {
            for (unsigned j=i+1;j<10;j++)
                if (data[j*2] || data[j*2+1]) return 0;
            break;
        }
        if (cp>=0xd800 && cp<=0xdbff) {
            if (++i>=10) return 0;
            uint32_t lo=(uint32_t)data[i*2] | (uint32_t)data[i*2+1]<<8;
            if (lo<0xdc00 || lo>0xdfff) return 0;
            cp=0x10000 + ((cp-0xd800)<<10) + (lo-0xdc00);
        } else if (cp>=0xdc00 && cp<=0xdfff) return 0;
        unsigned n=cp<0x80?1:cp<0x800?2:cp<0x10000?3:4;
        if (pos+n>=SPDS_MII_NAME) return 0;
        if (n==1) out[pos++]=(char)cp;
        else {
            unsigned shift=(n-1)*6;
            out[pos++]=(char)((n==2?0xc0:n==3?0xe0:0xf0)|(cp>>shift));
            while (shift) {
                shift-=6;
                out[pos++]=(char)(0x80|((cp>>shift)&63));
            }
        }
    }
    return 1;
}
static unsigned width(unsigned maximum) {
    unsigned w=0;
    do { w++; maximum>>=1; } while(maximum);
    return w;
}
static int compact_slot(SpdsMii *out,const uint8_t *p,unsigned version,
                        unsigned slot) {
    if (p[0]!=1) return 0;
    SpdsMii m;
    memset(&m,0,sizeof m);
    unsigned bit=0;
    for (unsigned f=0;f<SPDS_MII_FIELDS;f++) {
        unsigned max=version==1 && f==61 ? 95 : max_value[f];
        unsigned w=width(max), value=0;
        if (bit+w>272) return 0;
        for (unsigned k=0;k<w;k++,bit++)
            value|=((unsigned)(p[1+bit/8]>>(bit%8))&1u)<<k;
        m.features[f]=(uint8_t)(version==1 && f==61 ? 0 : value);
    }
    if (!from_utf16(m.name,p+35) || !from_utf16(m.creator,p+55))
        return 0;
    if (version==1 && m.features[51]) m.features[46]=1;
    if (!valid(&m)) return 0;
    m.from_gallery=1;
    m.slot=slot;
    packet_from_mii(&m);
    *out=m;
    return 1;
}
static int select_compact(SpdsMii *out,const uint8_t *p) {
    if (memcmp(p,"MDSC",4) || (get32(p+4)!=1 && get32(p+4)!=2) ||
        get32(p+12)!=COMPACT_BODY || get32(p+COMPACT_BODY)!=crc(p,COMPACT_BODY))
        return 0;
    SpdsMii chosen;
    int found=0;
    unsigned owners=0;
    for(unsigned i=0;i<SLOTS;i++) {
        const uint8_t *record=p+24+i*COMPACT_RECORD;
        if(record[0]>1) return 0;
        if(!record[0]) continue;
        SpdsMii next;
        if(!compact_slot(&next,record,get32(p+4),i)) return 0;
        if (next.features[51] && ++owners>1) return 0;
        if(!found || next.features[51]) {
            chosen=next;
            found=1;
        }
    }
    if(found) *out=chosen;
    return found;
}
static int legacy_bank(SpdsMii *out,const uint8_t *p) {
    if (memcmp(p,"MDSV",4) || get32(p+4)!=1 ||
        get32(p+12)!=LEGACY_BODY || get32(p+LEGACY_BODY)!=crc(p,LEGACY_BODY))
        return 0;
    int found=0;
    unsigned owners=0;
    SpdsMii chosen;
    for (unsigned i=0;i<SLOTS;i++) {
        const uint8_t *record=p+24+i*LEGACY_RECORD;
        if (record[0]>1) return 0;
        if (!record[0]) continue;
        SpdsMii next;
        if(!spds_mii_decode_packet(&next,record+1,SPDS_MII_PACKET)) return 0;
        next.from_gallery=1;
        next.slot=i;
        if (next.features[51] && ++owners>1) return 0;
        if (!found || next.features[51]) {
            chosen=next;
            found=1;
        }
    }
    if (!found) return 2; /* Valid bank, but gallery has no Miis. */
    *out=chosen;
    return 1;
}
int spds_mii_from_image(SpdsMii *out,const uint8_t *p,size_t len) {
    if(!out || !p) return 0;
    if(len==SPDS_MII_PACKET)
        return spds_mii_decode_packet(out,p,len);
    if(len==COMPACT_BYTES)
        return select_compact(out,p);
    if(len==LEGACY_BYTES) {
        SpdsMii a,b;
        int av=legacy_bank(&a,p), bv=legacy_bank(&b,p+LEGACY_BANK);
        if(av==0 && bv==0) return 0;
        /* Select most recent validated bank with wrap-safe generations. */
        int choose_b = !av ? 1 : !bv ? 0 :
          (get32(p+LEGACY_BANK+8)-get32(p+8)!=0 &&
           get32(p+LEGACY_BANK+8)-get32(p+8)<UINT32_C(0x80000000));
        const SpdsMii *chosen=choose_b?&b:&a;
        int status=choose_b?bv:av;
        if(status!=1) return 0;
        *out=*chosen;
        return 1;
    }
    return 0;
}
int spds_mii_from_file(SpdsMii *out,const char *path) {
    if(!out || !path || !*path) return 0;
    FILE *f=fopen(path,"rb");
    if(!f) return 0;
    if(fseek(f,0,SEEK_END)!=0) { fclose(f); return 0; }
    long len=ftell(f);
    if(len != (long)SPDS_MII_PACKET && len != COMPACT_BYTES &&
       len != LEGACY_BYTES) { fclose(f); return 0; }
    if(fseek(f,0,SEEK_SET)!=0) { fclose(f); return 0; }
    uint8_t *bytes=(uint8_t*)malloc((size_t)len);
    if(!bytes) { fclose(f); return 0; }
    int ok=fread(bytes,1,(size_t)len,f)==(size_t)len && !ferror(f);
    if(fclose(f)!=0) ok=0;
    if(ok) ok=spds_mii_from_image(out,bytes,(size_t)len);
    free(bytes);
    return ok;
}
static int cache_path(char *out,size_t n,const char *base) {
    if(!out || !base) return 0;
    int count=snprintf(out,n,"%s/mii.mds",base);
    return count>=0 && (size_t)count<n;
}
int spds_mii_cache_read(SpdsMii *out,const char *base) {
    char path[256];
    if(!cache_path(path,sizeof path,base)) return 0;
    return spds_mii_from_file(out,path);
}
int spds_mii_cache_write(const SpdsMii *mii,const char *base) {
    char path[256],tmp[256];
    if(!mii || !valid(mii) || !cache_path(path,sizeof path,base)) return 0;
    int count=snprintf(tmp,sizeof tmp,"%s/mii.tmp",base);
    if(count<0 || (size_t)count>=sizeof tmp) return 0;
    FILE *f=fopen(tmp,"wb");
    if(!f) return 0;
    int good=fwrite(mii->packet,1,SPDS_MII_PACKET,f)==SPDS_MII_PACKET;
    if(fclose(f)!=0) good=0;
    if(!good || rename(tmp,path)!=0) {
        remove(tmp);
        return 0;
    }
    return 1;
}
void spds_mii_radio_name(char out[SPDS_MII_NAME],const SpdsMii *mii) {
    memset(out,0,SPDS_MII_NAME);
    if(!mii || !valid(mii)) {
        memcpy(out,"DS Traveler",12);
        return;
    }
    unsigned j=0;
    for(unsigned i=0;i<SPDS_MII_NAME && mii->name[i] && j<SPDS_MII_NAME-1;i++) {
        unsigned c=(unsigned char)mii->name[i];
        out[j++]=(char)(c>=32 && c<127?c:'?');
    }
}
