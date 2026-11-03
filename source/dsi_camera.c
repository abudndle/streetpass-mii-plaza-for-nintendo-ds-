#include "dsi_camera.h"
#include "storage.h"

#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SPDS_CAMERA_NDMA 1
#define SPDS_CAMERA_PREVIEW_BYTES (256u * 192u * sizeof(u16))
#define SPDS_CAMERA_CAPTURE_WORDS (640u * 480u)
#define SPDS_CAMERA_CAPTURE_BYTES (SPDS_CAMERA_CAPTURE_WORDS * sizeof(u16))

static int wait_camera_dma(void)
{
    while (ndmaBusy(SPDS_CAMERA_NDMA))
        swiWaitForVBlank();
    return 0;
}

static int save_capture(const char *base, const u16 *pixels)
{
    char path[SPDS_PATH_MAX];
    int n = snprintf(path, sizeof(path), "%s/camera_last.yuv", base);
    if (n < 0 || n >= (int)sizeof(path))
        return -1;

    char temp[SPDS_PATH_MAX];
    n = snprintf(temp, sizeof(temp), "%s/camera_last.tmp", base);
    if (n < 0 || n >= (int)sizeof(temp))
        return -1;

    FILE *file = fopen(temp, "wb");
    if (!file)
        return -1;
    int ok = fwrite(pixels, 1, SPDS_CAMERA_CAPTURE_BYTES, file) ==
             SPDS_CAMERA_CAPTURE_BYTES;
    if (fclose(file) != 0)
        ok = 0;
    if (!ok || rename(temp, path) != 0) {
        remove(temp);
        return -1;
    }
    return 0;
}

int spds_dsi_camera_run(const char *save_dir)
{
    if (!save_dir || !*save_dir || !isDSiMode())
        return 0;

    u16 *capture = malloc(SPDS_CAMERA_CAPTURE_BYTES);
    if (!capture)
        return -1;

    PrintConsole camera_console;
    consoleSelect(NULL);
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    int preview_bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    if (preview_bg < 0) {
        free(capture);
        return -1;
    }

    consoleInit(&camera_console, 0, BgType_Text4bpp, BgSize_T_256x256,
                31, 0, true, true);
    printf("DSi CAMERA\n");
    printf("Initializing camera...\n");

    if (!cameraInit() || !cameraSelect(CAMERA_INNER)) {
        cameraDeinit();
        free(capture);
        return -1;
    }

    printf("A: Capture  B: Exit\n");
    printf("Saved as camera_last.yuv\n");
    printf("640 x 480 YUV422\n");

    int result = 0;
    bool running = true;
    while (running) {
        swiWaitForVBlank();
        if (!ndmaBusy(SPDS_CAMERA_NDMA) && !cameraTransferActive()) {
            cameraStartTransfer(bgGetGfxPtr(preview_bg),
                                MCUREG_APT_SEQ_CMD_PREVIEW,
                                SPDS_CAMERA_NDMA);
        }
        scanKeys();
        unsigned down = keysDown();
        if (down & KEY_B) {
            running = false;
        } else if (down & KEY_A) {
            wait_camera_dma();
            cameraStopTransfer();
            printf("\x1b[8;2HCapturing...       ");
            if (!cameraStartTransfer(capture, MCUREG_APT_SEQ_CMD_CAPTURE,
                                     SPDS_CAMERA_NDMA)) {
                result = -1;
                running = false;
                continue;
            }
            wait_camera_dma();
            cameraStopTransfer();
            if (save_capture(save_dir, capture) == 0) {
                printf("\x1b[8;2HCapture saved!     ");
                result = 1;
            } else {
                printf("\x1b[8;2HSave failed!       ");
                result = -1;
            }
            running = false;
        }
    }

    wait_camera_dma();
    cameraStopTransfer();
    cameraDeinit();
    free(capture);
    videoSetMode(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    return result;
}
