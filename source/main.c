#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "protocol.h"
#include "storage.h"
#include "wireless.h"
#include "mii_bridge.h"
#include "service_config.h"
#include "visitor.h"
#include "discovery.h"

static PrintConsole top_screen, bottom_screen;
static char save_dir[SPDS_PATH_MAX];
static SpdsRecord myself;
static SpdsPollStats last_poll;
static int storage_ok;
static unsigned total;
static SpdsWireless radio;
static SpdsServiceConfig service_cfg;
static SpdsMii linked_mii;
static int mii_loaded;
static SpdsVisitorStats visitor_stats;
static SpdsMii visitors_on_screen[3];
static int visitor_count;
static int auto_radio;
static unsigned role_check_frames;
static uint8_t outgoing[SPDS_WIRE_SIZE], incoming[SPDS_WIRE_SIZE];
static const char *radio_status(void) {
    if (radio.mode == SPDS_WIFI_OFF) return "Off";
    if (radio.phase == SPDS_PHASE_START) return "Starting";
    if (radio.phase == SPDS_PHASE_SCANNING) return "Scanning";
    if (radio.phase == SPDS_PHASE_CONNECTING) return "Connecting";
    if (radio.phase == SPDS_PHASE_LINKED)
        return radio.mode == SPDS_WIFI_HOST ? "Host" : "Joined";
    return "Radio error";
}

/* Import a previously saved Mii Maker DS profile without ever writing to
 * the original .sav. Only 7-bit nickname travels on SPDS v1 radio packets;
 * full Mii appearance remains in local mii.mds until a verified v2 exists.
 */
static int load_mii_from_sd(void) {
    if (!storage_ok || !spds_service_read(save_dir,&service_cfg)) return 0;
    auto_radio=service_cfg.enabled && service_cfg.radio_enabled;
    int imported=spds_service_load_mii(save_dir,&service_cfg,&linked_mii);
    if (imported<=0) return imported;
    mii_loaded=1;
    if(linked_mii.features[47]) { /* Mii Maker DS 'sharing allowed' policy */
        spds_mii_radio_name(myself.name,&linked_mii);
        if(spds_storage_save_profile(save_dir,&myself)!=0) return -1;
        if(spds_encode(&myself,outgoing)!=0) return -1;
        if(!spds_visitor_export(save_dir,&myself,&linked_mii)) return -1;
    } else {
        /* Never share a Mii with their sharing bit disabled. Remove any
         * stale full-appearance export after the user edits this policy. */
        char old[SPDS_PATH_MAX];
        int n=snprintf(old,sizeof old,"%s/outbox/visitor.spv",save_dir);
        if(n>=0 && n<(int)sizeof old) remove(old);
        strcpy(myself.name,"DS Traveler");
        if(spds_encode(&myself,outgoing)!=0 ||
           spds_storage_save_profile(save_dir,&myself)!=0) return -1;
    }
    return 1;
}
static void draw(void) {
    consoleSelect(&top_screen);
    printf("\x1b[2J\x1b[1;3HSTREETPASS MII PLAZA DS");
    printf("\x1b[4;2HIndependent homebrew prototype");
    printf("\x1b[6;2HVisitor: %-20s", myself.name);
    printf("\x1b[7;2HMaker Mii: %-20s",mii_loaded?linked_mii.name:"None linked");
    printf("\x1b[8;2HEncounters saved: %u", total);
    printf("\x1b[9;2HFull-Mii visitors: %u",visitor_stats.accepted);
    printf("\x1b[10;2HLocal radio: %-14s", radio_status());
    printf("\x1b[11;2HAuto scan: %s",auto_radio?"Enabled":"Disabled");
    printf("\x1b[12;2H3DS link: requires companion");
    printf("\x1b[14;2HBackground: menu process only");
    printf("\x1b[16;2HRecent saved visitor Miis:");
    if(!visitor_count) printf("\x1b[18;2HNo full-Mii visitors yet");
    else for(int i=0;i<visitor_count && i<3;i++) {
        printf("\x1b[%d;2H%-20.20s",18+i,
               visitors_on_screen[i].name);
    }
    consoleSelect(&bottom_screen);
    printf("\x1b[2J\x1b[1;2HSTREETPASS INBOX");
    printf("\x1b[4;2HStorage: %s", storage_ok ? "Ready" : "UNAVAILABLE");
    printf("\x1b[6;2HNew %u / duplicate %u",
            last_poll.accepted, last_poll.duplicates);
    printf("\x1b[8;2HInvalid: %u  I/O errors: %u",
            last_poll.invalid, last_poll.io_errors);
    printf("\x1b[11;2HA: Export your profile");
    printf("\x1b[13;2HX: Check SD inbox");
    printf("\x1b[14;2HY: Reload Mii Maker DS");
    printf("\x1b[15;2HB: Exit to loader");
    printf("\x1b[16;2HL: Host    R: Join/Scan");
    printf("\x1b[17;2HSELECT: Radio off");
    printf("\x1b[18;2HRX: %u TX: %u", radio.received, radio.sent);
    printf("\x1b[19;1HSD file exchange is a test");
    printf("\x1b[20;1Htransport, NOT radio StreetPass.");
    printf("\x1b[21;1HPlaza Mii cache: %s",mii_loaded?"Ready":"Not linked");
    printf("\x1b[22;1HSTART: Refresh status");
}
static void poll_now(void) {
    if (storage_ok) {
        if (spds_storage_poll(save_dir, myself.sender_id,
                              (uint32_t)time(NULL), &last_poll) != 0)
            last_poll.io_errors++;
        SpdsVisitorStats imported;
        if (spds_visitor_poll(save_dir,myself.sender_id,
                              (uint32_t)time(NULL),&imported) == 0) {
            visitor_stats=imported;
            last_poll.accepted+=imported.accepted;
            last_poll.duplicates+=imported.duplicates;
            last_poll.invalid+=imported.invalid;
            last_poll.io_errors+=imported.io_errors;
        } else last_poll.io_errors++;
        spds_storage_total(save_dir, &total);
        visitor_count=spds_visitor_list(save_dir,visitors_on_screen,3);
    }
}
int main(void) {
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&top_screen, 3, BgType_Text4bpp, BgSize_T_256x256,
                31, 0, true, true);
    consoleInit(&bottom_screen, 3, BgType_Text4bpp, BgSize_T_256x256,
                31, 0, false, true);
    memset(&myself, 0, sizeof(myself));
    strcpy(myself.name, "DS Traveler");
    const char *drive = NULL;
    if (fatInitDefault()) drive = fatGetDefaultDrive();
    if (drive && snprintf(save_dir, sizeof(save_dir),
                          "%sstreetpass-plaza", drive) <
                     (int)sizeof(save_dir) &&
        spds_storage_init(save_dir) == 0) {
        storage_ok = 1;
        if (spds_storage_load_profile(save_dir, &myself) != 0) {
            /* Local identity is random best-effort, not cryptographic. */
            unsigned seed = (unsigned)time(NULL) ^ (unsigned)REG_VCOUNT;
            srand(seed);
            myself.sender_id = ((uint64_t)(unsigned)rand() << 32) |
                               (uint64_t)(unsigned)rand();
            if (!myself.sender_id) myself.sender_id = 1;
            myself.timestamp = (uint32_t)time(NULL);
            myself.nonce = (uint32_t)rand();
            spds_storage_save_profile(save_dir, &myself);
        }
        if (spds_service_create(save_dir)) load_mii_from_sd();
        poll_now();
    }
    spds_encode(&myself, outgoing);
    draw();
    unsigned frames = 0;
    for (;;) {
        swiWaitForVBlank();
        scanKeys();
        unsigned down = keysDown();
        if (down & KEY_B) break;
        if ((down & KEY_L) && storage_ok) {
            auto_radio=0;
            spds_wifi_stop(&radio);
            spds_wifi_start(&radio, SPDS_WIFI_HOST);
            draw();
        }
        if ((down & KEY_R) && storage_ok) {
            auto_radio=0;
            spds_wifi_stop(&radio);
            spds_wifi_start(&radio, SPDS_WIFI_CLIENT);
            draw();
        }
        if (down & KEY_SELECT) {
            auto_radio=0;
            spds_wifi_stop(&radio);
            draw();
        }
        /* Only the actively running Plaza/menu can perform this polling.
         * Mode changes are intentionally slow to conserve startup overhead. */
        if(auto_radio && storage_ok && ++role_check_frames>=60) {
            role_check_frames=0;
            SpdsDiscoveryRole desired=spds_discovery_role(
                myself.sender_id,(uint32_t)time(NULL));
            enum SpdsWirelessMode target=
                desired==SPDS_DISCOVER_HOST ? SPDS_WIFI_HOST:SPDS_WIFI_CLIENT;
            if(radio.mode!=target || radio.phase==SPDS_PHASE_ERROR) {
                spds_wifi_stop(&radio);
                spds_wifi_start(&radio,target);
                draw();
            }
        }
        if (spds_wifi_tick(&radio, outgoing, incoming)) {
            SpdsRecord guest;
            if (spds_decode(&guest, incoming, SPDS_WIRE_SIZE) == 0) {
                int accepted = spds_storage_receive(
                    save_dir, &guest, myself.sender_id, (uint32_t)time(NULL));
                if (accepted > 0) {
                    ++total;
                    ++last_poll.accepted;
                }
                else if (accepted == 0) ++last_poll.duplicates;
                else ++last_poll.io_errors;
            } else ++last_poll.invalid;
            draw();
        }
        if (down & KEY_Y) {
            load_mii_from_sd();
            draw();
        }
        if (down & KEY_A) {
            if (storage_ok) {
                myself.timestamp = (uint32_t)time(NULL);
                if (spds_storage_save_profile(save_dir, &myself) != 0)
                    last_poll.io_errors++;
                if (mii_loaded && linked_mii.features[47] &&
                    !spds_visitor_export(save_dir,&myself,&linked_mii))
                    last_poll.io_errors++;
            }
            draw();
        }
        if ((down & (KEY_X | KEY_START)) || ++frames >= 300) {
            frames = 0;
            poll_now();
            draw();
        }
    }
    spds_wifi_stop(&radio);
    return 0;
}
