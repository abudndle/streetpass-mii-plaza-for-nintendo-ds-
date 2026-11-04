#include "plaza_pixels.h"
#include <assert.h>
#include <stdio.h>
static uint16_t framebuffer[SPDS_PLAZA_PIXELS_W*SPDS_PLAZA_PIXELS_H];
int main(void) {
    assert(spds_plaza_rgb555(31,0,0)==0x801fu);
    assert(spds_plaza_rgb555(99,0,0)==0x801fu);
    spds_plaza_pixels_fill(framebuffer,0x8001u);
    assert(framebuffer[0]==0x8001u && framebuffer[49151]==0x8001u);
    spds_plaza_pixels_rect(framebuffer,255,191,100,100,0x8002u);
    assert(framebuffer[49151]==0x8002u && framebuffer[49150]==0x8001u);
    spds_plaza_pixels_rect(framebuffer,256,0,1,1,0x8003u);
    assert(framebuffer[0]==0x8001u);
    spds_plaza_pixels_stage(framebuffer);
    assert(framebuffer[0]==spds_plaza_rgb555(15,22,29));
    assert(framebuffer[127*256]==spds_plaza_rgb555(26,26,22));
    assert(framebuffer[191*256]==spds_plaza_rgb555(13,19,12));
    uint16_t icon[4]={0x801fu,0x83e0u,0xfc00u,0xffffu};
    spds_plaza_pixels_fill(framebuffer,0x8000u);
    spds_plaza_pixels_blit(framebuffer,255,191,icon,2,2);
    assert(framebuffer[191u*256u+255u]==0x801fu);
    assert(framebuffer[191u*256u+254u]==0x8000u);
    spds_plaza_pixels_blit(framebuffer,0,0,icon,2,2);
    assert(framebuffer[0]==0x801fu);
    assert(framebuffer[1]==0x83e0u);
    assert(framebuffer[256]==0xfc00u);
    assert(framebuffer[257]==0xffffu);
    spds_plaza_pixels_blit(framebuffer,256,0,icon,2,2);
    spds_plaza_pixels_blit(framebuffer,0,192,icon,2,2);
    spds_plaza_pixels_blit(framebuffer,0,0,0,2,2);
    spds_plaza_pixels_blit(0,0,0,icon,2,2);
    assert(framebuffer[0]==0x801fu);
    spds_plaza_pixels_stage(0);
    puts("PASS: RGB555 stage fill, clipped rectangles, safe edges");
    return 0;
}
