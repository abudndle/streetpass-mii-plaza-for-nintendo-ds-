/* StreetPass Mii Plaza DS: original-content-free boot scaffold.
 * No claim of StreetPass compatibility or original-game parity.
 */
#include <nds.h>
#include <stdio.h>

static void draw_bottom(void) {
    consoleSelect(&bottomScreen);
    consoleClear();
    iprintf("StreetPass Mii Plaza DS\n");
    iprintf("----------------------\n\n");
    iprintf("Prototype navigation\n\n");
    iprintf("A: Show plaza status\n");
    iprintf("B: Return to menu\n");
    iprintf("START: About\n\n");
    iprintf("Original game assets are\n");
    iprintf("not included.\n");
}
static PrintConsole bottomScreen;
static PrintConsole topScreen;

int main(void) {
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    consoleSelect(&topScreen);
    iprintf("StreetPass Mii Plaza\n");
    iprintf("Nintendo DS / DSi\n\n");
    iprintf("Open-source port scaffold\n");
    iprintf("No wireless exchange yet\n");
    draw_bottom();
    while (1) {
        swiWaitForVBlank();
        scanKeys();
        const int pressed = keysDown();
        if (pressed & KEY_A) {
            consoleSelect(&bottomScreen);
            consoleClear();
            iprintf("PLAZA STATUS\n\n");
            iprintf("Visitors: 0\n");
            iprintf("Wireless: unavailable\n");
            iprintf("Save: not implemented\n\n");
            iprintf("Press B to return\n");
        }
        if (pressed & KEY_B) draw_bottom();
        if (pressed & KEY_START) {
            consoleSelect(&bottomScreen);
            consoleClear();
            iprintf("StreetPass Mii Plaza DS\n\n");
            iprintf("Development build only\n");
            iprintf("No Nintendo assets\n");
            iprintf("No 3DS protocol support\n\n");
            iprintf("Press B to return\n");
        }
    }
}
