#ifndef SPDS_SERVICE_CONFIG_H
#define SPDS_SERVICE_CONFIG_H
#include "mii_bridge.h"
#include "storage.h"

/* Menu-independent directory contract. A menu must opt into the module;
 * installing service.cfg never runs background code by itself. */
#define SPDS_SERVICE_CONFIG "service.cfg"
#define SPDS_SERVICE_DEFAULT_MII ""
typedef struct {
    int enabled;       /* permits polling when the host process is active */
    int radio_enabled; /* opt-in: no radio transmission just from SD presence */
    char mii_source[SPDS_PATH_MAX]; /* explicitly selected source .sav/.mds */
} SpdsServiceConfig;

/* Creates the configuration file if absent; never overwrites user choices. */
int spds_service_create(const char *service_dir);
int spds_service_read(const char *service_dir,SpdsServiceConfig *config);
/* Read-only Mii Maker save import, cache as mii.mds in service directory.
 * Returns 1 on imported/cached Mii, 0 if none, -1 on invalid settings. */
int spds_service_load_mii(const char *service_dir,
                          const SpdsServiceConfig *config,SpdsMii *out);
#endif
