#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "protocol.h"
#include "storage.h"

static PrintConsole top_screen, bottom_screen;
static char save_dir[SPDS_PATH_MAX];
static SpdsRecord myself;
static SpdsPollStats last_poll;
static int storage_ok;
static unsigned total;

static void draw(void) {
    consoleSelect(&top_screen);
    iprintf("\x1b[2J\x1b[1;3HSTREETPASS MII PLAZA DS");
    iprintf("\x1b[4;2HIndependent homebrew prototype");
    iprintf("\x1b[6;2HVisitor: %-20s", myself.name);
    iprintf("\x1b[8;2HEncounters saved: %u", total);
    iprintf("\x1b[10;2HRadio exchange: NOT READY");
    iprintf("\x1b[12;2H3DS link: requires companion");
    iprintf("\x1b[14;2HBackground: menu process only");
    consoleSelect(&bottom_screen);
    iprintf("\x1b[2J\x1b[1;2HSTREETPASS INBOX");
    iprintf("\x1b[4;2HStorage: %s", storage_ok ? "Ready" : "UNAVAILABLE");
    iprintf("\x1b[6;2HNew %u / duplicate %u",
            last_poll.accepted, last_poll.duplicates);
    iprintf("\x1b[8;2HInvalid: %u  I/O errors: %u",
            last_poll.invalid, last_poll.io_errors);
    iprintf("\x1b[11;2HA: Export your profile");
    iprintf("\x1b[13;2HX: Check SD inbox");
    iprintf("\x1b[15;2HB: Exit to loader");
    iprintf("\x1b[19;1HSD file exchange is a test");
    iprintf("\x1b[20;1Htransport, NOT radio StreetPass.");
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
    draw();
    unsigned frames = 0;
    for (;;) {
        swiWaitForVBlank();
        scanKeys();
        unsigned down = keysDown();
        if (down & KEY_B) break;
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
    return 0;
}
