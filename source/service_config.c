#include "service_config.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * No process is started by writing this file. It only configures a running
 * Plaza app or a cooperating homebrew menu that explicitly uses these APIs.
 */
static int path(char out[SPDS_PATH_MAX],const char *base,const char *filename) {
    if (!base || !filename || !out) return 0;
    int count=snprintf(out,SPDS_PATH_MAX,"%s/%s",base,filename);
    return count>=0 && count<SPDS_PATH_MAX;
}
int spds_service_create(const char *base) {
    if(!base || spds_storage_init(base)) return 0;
    char configpath[SPDS_PATH_MAX];
    if(!path(configpath,base,SPDS_SERVICE_CONFIG)) return 0;
    FILE *f=fopen(configpath,"rb");
    if(f) { fclose(f); return 1; }
    f=fopen(configpath,"wb");
    if(!f) return 0;
    const char content[] =
      "# StreetPassDS homebrew service settings v1\n"
      "# This file cannot start a background process by itself.\n"
      "# Your launcher/game must explicitly integrate the service.\n"
      "enabled=1\n"
      "radio_enabled=0\n"
      "# Set mii_source to your Mii Maker DS 8192/65536-byte .sav or 134-byte .mds.\n"
      "# Example on flashcard: mii_source=fat:/roms/mii-ds.sav\n"
      "# Example on DSi SD: mii_source=sd:/roms/mii-ds.sav\n"
      "mii_source=\n";
    int good=fwrite(content,1,sizeof(content)-1,f)==sizeof(content)-1;
    if(fclose(f)!=0) good=0;
    if(!good) { remove(configpath); return 0; }
    return 1;
}
static char *trim(char *line) {
    while(*line && isspace((unsigned char)*line)) ++line;
    size_t n=strlen(line);
    while(n && isspace((unsigned char)line[n-1])) line[--n]=0;
    return line;
}
int spds_service_read(const char *base,SpdsServiceConfig *cfg) {
    if(!base || !cfg) return 0;
    char configpath[SPDS_PATH_MAX];
    if(!path(configpath,base,SPDS_SERVICE_CONFIG)) return 0;
    FILE *f=fopen(configpath,"rb");
    if(!f) return 0;
    SpdsServiceConfig tmp;
    memset(&tmp,0,sizeof tmp);
    char line[SPDS_PATH_MAX+48];
    int good=1;
    while(fgets(line,sizeof line,f)) {
        /* Reject silently truncated lines rather than importing wrong paths. */
        size_t n=strlen(line);
        if(n==sizeof line-1 && line[n-1]!='\n' && !feof(f)) {
            good=0; break;
        }
        char *p=trim(line);
        if(!*p || *p=='#') continue;
        char *equal=strchr(p,'=');
        if(!equal) { good=0; break; }
        *equal=0;
        char *key=trim(p),*value=trim(equal+1);
        if(!strcmp(key,"enabled")) {
            if(strcmp(value,"0") && strcmp(value,"1")) { good=0; break; }
            tmp.enabled=*value=='1';
        } else if(!strcmp(key,"radio_enabled")) {
            if(strcmp(value,"0") && strcmp(value,"1")) { good=0; break; }
            tmp.radio_enabled=*value=='1';
        } else if(!strcmp(key,"mii_source")) {
            if(strlen(value)>=sizeof tmp.mii_source) { good=0; break; }
            memcpy(tmp.mii_source,value,strlen(value)+1);
        } /* ignore future extension keys */
    }
    if(ferror(f)) good=0;
    fclose(f);
    if(!good) return 0;
    *cfg=tmp;
    return 1;
}
int spds_service_load_mii(const char *base,
                          const SpdsServiceConfig *config,SpdsMii *out) {
    if(!base || !config || !out) return -1;
    SpdsMii chosen;
    if(config->mii_source[0]) {
        char candidate[SPDS_PATH_MAX];
        /* Allow explicit FAT/SD absolute paths; relative paths are service
         * directory-relative. This avoids depending on launcher cwd. */
        int absolute=strchr(config->mii_source,':')!=NULL ||
                     config->mii_source[0]=='/';
        int len=absolute?snprintf(candidate,sizeof candidate,"%s",
                                 config->mii_source):
                 snprintf(candidate,sizeof candidate,"%s/%s",base,
                          config->mii_source);
        if(len<0 || len>=(int)sizeof candidate) return -1;
        if(spds_mii_from_file(&chosen,candidate)) {
            if(!spds_mii_cache_write(&chosen,base)) return -1;
            *out=chosen;
            return 1;
        }
    }
    if(spds_mii_cache_read(&chosen,base)) {
        *out=chosen;
        return 1;
    }
    return 0;
}
