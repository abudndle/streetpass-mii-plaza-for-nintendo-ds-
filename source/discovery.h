#ifndef SPDS_DISCOVERY_H
#define SPDS_DISCOVERY_H
#include <stdint.h>
/* Pure hardware-independent role scheduler for optional local discovery.
 * The service is NOT resident outside the running app or cooperative menu.
 * A random-looking period per sender ID avoids permanent host-host and
 * client-client lockstep, but is not cryptographic or a connection guarantee.
 */
typedef enum { SPDS_DISCOVER_HOST=1, SPDS_DISCOVER_CLIENT=2 } SpdsDiscoveryRole;
SpdsDiscoveryRole spds_discovery_role(uint64_t sender_id,uint32_t seconds);
#endif
