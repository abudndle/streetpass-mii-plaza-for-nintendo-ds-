#include "storage.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(int argc,char **argv) {
    assert(argc==3);
    const char *mode=argv[1],*dir=argv[2];
    assert(spds_storage_init(dir)==0);
    if(strcmp(mode,"write")==0) {
        SpdsRecord profile={0xabcddcba11223344ULL,1701000000u,7u,"Local Mii"};
        SpdsRecord visitor={0x1020304050607080ULL,1701000001u,8u,"Example Guest"};
        assert(spds_storage_save_profile(dir,&profile)==0);
        assert(spds_storage_receive(dir,&visitor,profile.sender_id,1701000002u)==1);
        puts("PASS writer persisted profile and visitor");
        return 0;
    }
    if(strcmp(mode,"read")==0) {
        SpdsRecord profile,duplicate={0x1020304050607080ULL,1701000010u,9u,"Duplicate"};
        unsigned total=0,waiting=0;
        assert(spds_storage_load_profile(dir,&profile)==0);
        assert(profile.sender_id==0xabcddcba11223344ULL);
        assert(strcmp(profile.name,"Local Mii")==0);
        assert(spds_storage_total(dir,&total)==0&&total==1);
        assert(spds_storage_queue_count(dir,&waiting)==0&&waiting==1);
        assert(spds_storage_receive(dir,&duplicate,profile.sender_id,1701000010u)==0);
        assert(spds_storage_queue_count(dir,&waiting)==0&&waiting==1);
        puts("PASS new process recovered profile, visitor queue and duplicate cooldown");
        return 0;
    }
    return 2;
}
