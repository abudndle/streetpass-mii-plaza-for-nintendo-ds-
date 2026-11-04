#ifndef SPDS_PLAZA_PIXELS_HEADER_H
#define SPDS_PLAZA_PIXELS_HEADER_H
#include <stdint.h>
/* Platform-neutral RGB555 drawing surface for a 256x192 DS top screen.
 * The caller owns VRAM and controls whether the surface is visible. */
#define SPDS_PLAZA_PIXELS_W 256u
#define SPDS_PLAZA_PIXELS_H 192u
uint16_t spds_plaza_rgb555(unsigned r,unsigned g,unsigned b);
void spds_plaza_pixels_fill(volatile uint16_t *pixels,uint16_t color);
void spds_plaza_pixels_rect(volatile uint16_t *pixels,
                            unsigned x,unsigned y,unsigned w,unsigned h,
                            uint16_t color);
/* Copy RGB555 pixels into a 256-wide bitmap, clipped to the 256x192
 * visible screen; source stride is the original image width. */
void spds_plaza_pixels_blit(volatile uint16_t *pixels,
                            unsigned x,unsigned y,
                            const uint16_t *source,unsigned width,unsigned height);
void spds_plaza_pixels_stage(volatile uint16_t *pixels);
#endif
