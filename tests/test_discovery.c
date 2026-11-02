#include "discovery.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    unsigned overlap=0;
    for(unsigned t=0;t<360;t++) {
        SpdsDiscoveryRole a=spds_discovery_role(17,t);
        SpdsDiscoveryRole b=spds_discovery_role(99877,t);
        assert(a==SPDS_DISCOVER_HOST || a==SPDS_DISCOVER_CLIENT);
        assert(b==SPDS_DISCOVER_HOST || b==SPDS_DISCOVER_CLIENT);
        if(a!=b) overlap++;
    }
    assert(overlap>90);
    /* Same identity can't be treated as proof of different devices. */
    assert(spds_discovery_role(17,100)==spds_discovery_role(17,100));
    puts("PASS: bounded alternating host/client discovery roles");
    return 0;
}
