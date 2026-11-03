#ifndef SPDS_DSI_CAMERA_H
#define SPDS_DSI_CAMERA_H

/* DSi-only camera probe/capture UI. Returns 1 if a photo was saved,
 * 0 if unavailable/cancelled, and -1 on an I/O or camera error. */
int spds_dsi_camera_run(const char *save_dir);

#endif
