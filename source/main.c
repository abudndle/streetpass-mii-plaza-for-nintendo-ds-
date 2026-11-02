#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "protocol.h"
#include "storage.h"
#include "wireless.h"

static PrintConsole top_screen, bottom_screen;
static char save_dir[SPDS_PATH_MAX];
static SpdsRecord myself;
static SpdsPollStats last_poll;
static int storage_ok;
static unsigned total;
static SpdsWireless radio;
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

static void draw(void) {
    consoleSelect(&top_screen);
    printf("\x1b[2J\x1b[1;3HSTREETPASS MII PLAZA DS");
    printf("\x1b[4;2HIndependent homebrew prototype");
    printf("\x1b[6;2HVisitor: %-20s", myself.name);
    printf("\x1b[8;2HEncounters saved: %u", total);
    printf("\x1b[10;2HLocal radio: %-14s", radio_status());
    printf("\x1b[12;2H3DS link: requires companion");
    printf("\x1b[14;2HBackground: menu process only");
    consoleSelect(&bottom_screen);
    printf("\x1b[2J\x1b[1;2HSTREETPASS INBOX");
    printf("\x1b[4;2HStorage: %s", storage_ok ? "Ready" : "UNAVAILABLE");
    printf("\x1b[6;2HNew %u / duplicate %u",
            last_poll.accepted, last_poll.duplicates);
    printf("\x1b[8;2HInvalid: %u  I/O errors: %u",
            last_poll.invalid, last_poll.io_errors);
    printf("\x1b[11;2HA: Export your profile");
    printf("\x1b[13;2HX: Check SD inbox");
    printf("\x1b[15;2HB: Exit to loader");
    printf("\x1b[16;2HL: Host    R: Join/Scan");
    printf("\x1b[17;2HSELECT: Radio off");
    printf("\x1b[18;2HRX: %u TX: %u", radio.received, radio.sent);
    printf("\x1b[19;1HSD file exchange is a test");
    printf("\x1b[20;1Htransport, NOT radio StreetPass.");
}
static void poll_now(void) {
    if (storage_ok) {
        if (spds_storage_poll(save_dir, myself.sender_id,
                              (uint32_t)time(NULL), &last_poll) != 0)
            last_poll.io_errors++;
        spds_storage_total(save_dir, &total);
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
            spds_wifi_stop(&radio);
            spds_wifi_start(&radio, SPDS_WIFI_HOST);
            draw();
        }
        if ((down & KEY_R) && storage_ok) {
            spds_wifi_stop(&radio);
            spds_wifi_start(&radio, SPDS_WIFI_CLIENT);
            draw();
        }
        if (down & KEY_SELECT) {
            spds_wifi_stop(&radio);
            draw();
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
        if (down & KEY_A) {
            if (storage_ok) {
                myself.timestamp = (uint32_t)time(NULL);
                if (spds_storage_save_profile(save_dir, &myself) != 0)
                    last_poll.io_errors++;
            }
            draw();
        }
        if ((down & KEY_X) || ++frames >= 300) {
            frames = 0;
            poll_now();
            draw();
        }
    }
    spds_wifi_stop(&radio);
    return 0;
}
