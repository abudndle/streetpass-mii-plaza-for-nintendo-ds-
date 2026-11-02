#include "radio_ack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    uint8_t packet[SPDS_ACK_SIZE], corrupt[SPDS_ACK_SIZE];
    SpdsAck reply={0};
    SpdsAckTracker pending={0};
    assert(spds_ack_encode(packet,0x1122334455667788ull,0x13572468u));
    assert(spds_ack_decode(&reply,packet,sizeof(packet)));
    assert(reply.sender_id==0x1122334455667788ull && reply.nonce==0x13572468u);
    assert(!spds_ack_encode(packet,0,1));
    assert(!spds_ack_decode(&reply,packet,sizeof(packet)-1));
    assert(spds_ack_encode(packet,0x1122334455667788ull,0x13572468u));
    memcpy(corrupt,packet,sizeof(packet));
    corrupt[11]^=0x80;
    assert(!spds_ack_decode(&reply,corrupt,sizeof(corrupt)));
    memcpy(corrupt,packet,sizeof(packet));
    corrupt[4]=2;
    assert(!spds_ack_decode(&reply,corrupt,sizeof(corrupt)));
    memcpy(corrupt,packet,sizeof(packet));
    corrupt[6]=1;
    assert(!spds_ack_decode(&reply,corrupt,sizeof(corrupt)));
    assert(!spds_ack_tracker_consume(&pending,packet,sizeof(packet)));
    assert(spds_ack_tracker_track(&pending,0x1122334455667788ull,0x13572468u));
    assert(!spds_ack_tracker_consume(&pending,corrupt,sizeof(corrupt)));
    assert(spds_ack_tracker_consume(&pending,packet,sizeof(packet)));
    assert(!spds_ack_tracker_consume(&pending,packet,sizeof(packet)));
    assert(pending.confirmed==1 && pending.invalid==1);
    assert(spds_ack_tracker_track(&pending,0x1122334455667788ull,5));
    assert(!spds_ack_tracker_consume(&pending,packet,sizeof(packet)));
    assert(pending.unmatched==1);
    puts("PASS: SPDA frame CRC, strict version, matching and replay rejection");
    return 0;
}
