#include "visitor.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int path(char out[SPDS_PATH_MAX],const char *base,const char *name) {
    int n=snprintf(out,SPDS_PATH_MAX,"%s/%s",base,name);
    return n>=0 && n<(int)SPDS_PATH_MAX;
}
static void put32(uint8_t *p,uint32_t v) {
    for(unsigned i=0;i<4;i++) p[i]=(uint8_t)(v>>(8*i));
}
static uint32_t get32(const uint8_t *p) {
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|
           (uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
int spds_visitor_encode(uint8_t out[SPDS_VISITOR_SIZE],
                        const SpdsRecord *record,const SpdsMii *mii) {
    if(!out || !record || !mii || !mii->features[47]) return 0;
    SpdsMii valid;
    if(!spds_mii_decode_packet(&valid,mii->packet,SPDS_MII_PACKET) ||
       !valid.features[47]) return 0;
    char wanted[SPDS_MII_NAME];
    spds_mii_radio_name(wanted,&valid);
    if(strcmp(record->name,wanted)!=0) return 0;
    memset(out,0,SPDS_VISITOR_SIZE);
    memcpy(out,"SPDV",4);
    out[4]=1;
    if(spds_encode(record,out+8)!=0) return 0;
    memcpy(out+72,valid.packet,SPDS_MII_PACKET);
    put32(out+206,spds_crc32(out,206));
    return 1;
}
int spds_visitor_decode(SpdsRecord *record,SpdsMii *mii,
                        const uint8_t *data,size_t n) {
    if(!record || !mii || !data || n!=SPDS_VISITOR_SIZE ||
       memcmp(data,"SPDV",4) || data[4]!=1 ||
       data[5]!=0 || data[6]!=0 || data[7]!=0 ||
       get32(data+206)!=spds_crc32(data,206)) return 0;
    SpdsRecord packet;
    SpdsMii avatar;
    if(spds_decode(&packet,data+8,SPDS_WIRE_SIZE)!=0 ||
       !spds_mii_decode_packet(&avatar,data+72,SPDS_MII_PACKET) ||
       !avatar.features[47]) return 0;
    char nickname[SPDS_MII_NAME];
    spds_mii_radio_name(nickname,&avatar);
    if(strcmp(packet.name,nickname)!=0) return 0;
    *record=packet;
    *mii=avatar;
    return 1;
}
static int write_atomic(const char *dest,const char *temp,const uint8_t *data,size_t size) {
    FILE *f=fopen(temp,"wb");
    if(!f) return 0;
    int good=fwrite(data,1,size,f)==size;
    if(fclose(f)!=0) good=0;
    if(!good || rename(temp,dest)!=0) {
        remove(temp);
        return 0;
    }
    return 1;
}
int spds_visitor_export(const char *base,const SpdsRecord *profile,const SpdsMii *mii) {
    char dest[SPDS_PATH_MAX],temp[SPDS_PATH_MAX];
    uint8_t data[SPDS_VISITOR_SIZE];
    if(!base || !path(dest,base,"outbox/visitor.spv") ||
       !path(temp,base,"outbox/visitor.tmp") ||
       !spds_visitor_encode(data,profile,mii)) return 0;
    return write_atomic(dest,temp,data,sizeof data);
}
static int save_received(const char *base,const SpdsRecord *profile,const SpdsMii *mii) {
    char dir[SPDS_PATH_MAX],file[SPDS_PATH_MAX],tmp[SPDS_PATH_MAX];
    char name[48];
    if(!path(dir,base,"visitors")) return 0;
    if(mkdir(dir,0777)!=0 && errno!=EEXIST) return 0;
    int len=snprintf(name,sizeof name,"%016llx.mds",
                     (unsigned long long)profile->sender_id);
    if(len<0 || len>=(int)sizeof name || !path(file,dir,name) ||
       !path(tmp,dir,"pending.tmp")) return 0;
    /* Personal/gold crown belongs only to the original console; visitors
     * are received Miis, not another console's Personal Mii. */
    uint8_t packet[SPDS_MII_PACKET];
    memcpy(packet,mii->packet,sizeof packet);
    packet[68+51]=0;
    packet[68+61]=2;
    put32(packet+130,spds_crc32(packet,130));
    return write_atomic(file,tmp,packet,sizeof packet);
}
int spds_visitor_read(const char *base,uint64_t sender_id,SpdsMii *out) {
    if(!base || !sender_id || !out) return 0;
    char dir[SPDS_PATH_MAX],file[SPDS_PATH_MAX],name[48];
    if(!path(dir,base,"visitors")) return 0;
    snprintf(name,sizeof name,"%016llx.mds",(unsigned long long)sender_id);
    if(!path(file,dir,name)) return 0;
    return spds_mii_from_file(out,file);
}
static uint64_t parse_id(const char *filename) {
    if(strlen(filename)!=20 || strcmp(filename+16,".mds")!=0) return 0;
    uint64_t id=0;
    for(unsigned i=0;i<16;i++) {
        char c=filename[i];
        unsigned digit= c>='0' && c<='9' ? (unsigned)(c-'0') :
                        c>='a' && c<='f' ? (unsigned)(c-'a'+10) : 16u;
        if(digit>=16) return 0;
        id=(id<<4)|digit;
    }
    return id;
}
int spds_visitor_list_page(const char *base,unsigned offset,SpdsMii *out,unsigned cap) {
    if(!base || (cap && !out)) return -1;
    char dirpath[SPDS_PATH_MAX];
    if(!path(dirpath,base,"visitors")) return -1;
    DIR *d=opendir(dirpath);
    if(!d) return errno==ENOENT?0:-1;
    unsigned count=0,scanned=0;
    struct dirent *entry;
    while((entry=readdir(d))!=NULL && scanned<512) {
        uint64_t id=parse_id(entry->d_name);
        if(!id) continue;
        scanned++;
        SpdsMii candidate;
        if(!spds_visitor_read(base,id,&candidate)) continue;
        if(count>=offset && count-offset<cap) out[count-offset]=candidate;
        count++;
    }
    closedir(d);
    return (int)count;
}
int spds_visitor_list(const char *base,SpdsMii *out,unsigned cap) {
    return spds_visitor_list_page(base,0,out,cap);
}
int spds_visitor_poll(const char *base,uint64_t own_id,uint32_t now,
                      SpdsVisitorStats *stats) {
    if(!base || !stats) return -1;
    memset(stats,0,sizeof(*stats));
    char directory[SPDS_PATH_MAX],full[SPDS_PATH_MAX];
    if(!path(directory,base,"inbox")) return -1;
    DIR *dir=opendir(directory);
    if(!dir) return -1;
    struct dirent *entry;
    unsigned limit=0;
    while((entry=readdir(dir))!=NULL && limit<64) {
        size_t len=strlen(entry->d_name);
        if(len<5 || strcmp(entry->d_name+len-4,".spv")!=0) continue;
        if(strchr(entry->d_name,'/') || strchr(entry->d_name,'\\') ||
           !path(full,directory,entry->d_name)) {
            stats->invalid++;
            continue;
        }
        ++limit;
        FILE *f=fopen(full,"rb");
        if(!f) { stats->io_errors++; continue; }
        uint8_t buffer[SPDS_VISITOR_SIZE];
        size_t read=fread(buffer,1,sizeof buffer,f);
        int trailing=fgetc(f);
        int failed=ferror(f);
        if(fclose(f)!=0) failed=1;
        SpdsRecord profile;
        SpdsMii avatar;
        if(failed || read!=sizeof buffer || trailing!=EOF ||
           !spds_visitor_decode(&profile,&avatar,buffer,sizeof buffer)) {
            stats->invalid++; continue;
        }
        int accepted=spds_storage_receive(base,&profile,own_id,now);
        if(accepted<0) { stats->io_errors++; continue; }
        if(accepted==SPDS_STORAGE_QUEUE_FULL) { stats->queue_full++; continue; }
        if(accepted==0) stats->duplicates++;
        else if(!save_received(base,&profile,&avatar)) {
            /* Log written, retry could be considered duplicate: report error
             * and preserve file for explicit/manual recovery. */
            stats->io_errors++;
            continue;
        } else stats->accepted++;
        if(remove(full)!=0) stats->io_errors++;
    }
    closedir(dir);
    return 0;
}
