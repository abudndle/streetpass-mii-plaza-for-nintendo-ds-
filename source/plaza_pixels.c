#include "plaza_pixels.h"
#include <stddef.h>
uint16_t spds_plaza_rgb555(unsigned r,unsigned g,unsigned b) {
    if(r>31u)r=31u;
    if(g>31u)g=31u;
    if(b>31u)b=31u;
    return (uint16_t)(0x8000u | r | (g<<5) | (b<<10));
}
void spds_plaza_pixels_fill(volatile uint16_t *pixels,uint16_t color) {
    if(!pixels)return;
    for(unsigned i=0;i<SPDS_PLAZA_PIXELS_W*SPDS_PLAZA_PIXELS_H;i++)
        pixels[i]=color;
}
void spds_plaza_pixels_rect(volatile uint16_t *pixels,
                            unsigned x,unsigned y,unsigned w,unsigned h,
                            uint16_t color) {
    if(!pixels || x>=SPDS_PLAZA_PIXELS_W || y>=SPDS_PLAZA_PIXELS_H)return;
    if(w>SPDS_PLAZA_PIXELS_W-x)w=SPDS_PLAZA_PIXELS_W-x;
    if(h>SPDS_PLAZA_PIXELS_H-y)h=SPDS_PLAZA_PIXELS_H-y;
    for(unsigned row=0;row<h;row++)
        for(unsigned col=0;col<w;col++)
            pixels[(y+row)*SPDS_PLAZA_PIXELS_W+x+col]=color;
}
/* Original UI/model textures are NOT present in public builds. This
 * supplies only a simple stage background for eventual private assets. */
void spds_plaza_pixels_stage(volatile uint16_t *pixels) {
    if(!pixels)return;
    spds_plaza_pixels_fill(pixels,spds_plaza_rgb555(15,22,29));
    spds_plaza_pixels_rect(pixels,0,128,256,64,spds_plaza_rgb555(13,19,12));
    spds_plaza_pixels_rect(pixels,0,124,256,4,spds_plaza_rgb555(26,26,22));
}
