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
    puts("PASS: companion C codec 4-hop identity, TTL, CRC, length");
    return 0;
}
