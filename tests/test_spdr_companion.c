#include "spdr_companion.h"
#include "relay.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    SpdrCompanionVisitor origin={123,1700000000,456,1700000000,4,"Visitor"},got;
    uint8_t packet[80],next[80];
    assert(spdr_companion_encode(packet,&origin));
    for(unsigned hop=4;;--hop) {
        assert(spdr_companion_decode(&got,packet,sizeof packet));
        assert(got.sender_id==123 && got.nonce==456 && got.hops==hop);
        assert(got.originated==1700000000 && !strcmp(got.name,"Visitor"));
        if(!hop) break;
        assert(spdr_companion_forward(next,packet,sizeof packet));
        memcpy(packet,next,sizeof packet);
    }
    assert(!spdr_companion_forward(next,packet,sizeof packet));
    assert(!spdr_companion_decode(&got,packet,79));
    packet[20]^=1;
    assert(!spdr_companion_decode(&got,packet,sizeof packet));
    /* Multiple independent DS/3DS companion identities must not alias.
     * A relay's hop budget must decrease without changing the sender. */
    SpdrCompanionVisitor peers[] = {
        {0x1111222233334444ULL,1700000001u,11u,1700000001u,3u,"DS Alpha"},
        {0x5555666677778888ULL,1700000002u,22u,1700000002u,3u,"3DS Beta"},
        {0x9999aaaabbbbccccULL,1700000003u,33u,1700000003u,3u,"3DS Gamma"}
    };
    for(unsigned i=0;i<sizeof peers/sizeof peers[0];++i) {
        assert(spdr_companion_encode(packet,&peers[i]));
        for(unsigned hop=peers[i].hops;;--hop) {
            assert(spdr_companion_decode(&got,packet,sizeof packet));
            assert(got.sender_id==peers[i].sender_id);
            assert(got.timestamp==peers[i].timestamp);
            assert(got.nonce==peers[i].nonce);
            assert(got.originated==peers[i].originated);
            assert(got.hops==hop);
            assert(strcmp(got.name,peers[i].name)==0);
            if(!hop) break;
            assert(spdr_companion_forward(next,packet,sizeof packet));
            memcpy(packet,next,sizeof packet);
        }
        assert(!spdr_companion_forward(next,packet,sizeof packet));
    }
    /* The codec rejects an invalid TTL and unterminated names. */
    SpdrCompanionVisitor invalid=peers[0];
    invalid.hops=SPDR_COMPANION_MAX_HOPS+1u;
    assert(!spdr_companion_encode(packet,&invalid));
    invalid=peers[0];
    memset(invalid.name,'X',sizeof invalid.name);
    assert(!spdr_companion_encode(packet,&invalid));
    puts("PASS: companion C codec multi-peer identities, 4-hop TTL, CRC, length");
    return 0;
}
