#include "plaza_3d.h"
#include "storage.h"

#include <NEMain.h>
#include <stdio.h>
#include <string.h>

#define SPDS_ROOM_VISITORS 3

static NE_Camera *room_camera;
static NE_Model *visitor_models[SPDS_ROOM_VISITORS];
static NE_Animation *visitor_idle;
static int renderer_active;
static unsigned visible_visitors;
static unsigned selected_visitor;
static unsigned animation_frame;

static void restore_2d_display(void)
{
    irqSet(IRQ_VBLANK, NULL);
    irqSet(IRQ_HBLANK, NULL);
    irqDisable(IRQ_HBLANK);
    videoSetMode(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
}

static void draw_room(void *unused)
{
    (void)unused;
    if (!renderer_active || !room_camera)
        return;

    NE_CameraUse(room_camera);
    for (unsigned i = 0; i < visible_visitors; ++i) {
        if (visitor_models[i])
            NE_ModelDraw(visitor_models[i]);
    }
}

int spds_plaza_3d_enter(const char *save_dir)
{
    char mesh_path[SPDS_PATH_MAX];
    char anim_path[SPDS_PATH_MAX];

    if (!save_dir || renderer_active)
        return 0;

    int mesh_len = snprintf(mesh_path, sizeof(mesh_path),
                            "%s/models/visitor.dsm", save_dir);
    int anim_len = snprintf(anim_path, sizeof(anim_path),
                            "%s/models/idle.dsa", save_dir);
    if (mesh_len < 0 || mesh_len >= (int)sizeof(mesh_path) ||
        anim_len < 0 || anim_len >= (int)sizeof(anim_path))
        return 0;

    /* Leave the existing 2D application untouched unless both model files
     * exist. This allows the same ROM to boot without optional art assets. */
    FILE *mesh_file = fopen(mesh_path, "rb");
    if (!mesh_file)
        return 0;
    fclose(mesh_file);
    FILE *anim_file = fopen(anim_path, "rb");
    if (!anim_file)
        return 0;
    fclose(anim_file);

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);
    if (NE_Init3D() != 0) {
        restore_2d_display();
        return 0;
    }

    room_camera = NE_CameraCreate();
    if (!room_camera) {
        NE_End();
        restore_2d_display();
        return 0;
    }
    NE_CameraSet(room_camera, 0.0f, 1.25f, 5.5f,
                 0.0f, 0.7f, 0.0f, 0.0f, 1.0f, 0.0f);

    visitor_idle = NE_AnimationCreate();
    if (!visitor_idle || !NE_AnimationLoadFAT(visitor_idle, anim_path)) {
        NE_End();
        restore_2d_display();
        room_camera = NULL;
        visitor_idle = NULL;
        return 0;
    }

    visitor_models[0] = NE_ModelCreate(NE_Animated);
    if (!visitor_models[0] ||
        !NE_ModelLoadDSMFAT(visitor_models[0], mesh_path)) {
        NE_End();
        restore_2d_display();
        room_camera = NULL;
        visitor_idle = NULL;
        memset(visitor_models, 0, sizeof(visitor_models));
        return 0;
    }
    NE_ModelSetAnimation(visitor_models[0], visitor_idle);
    NE_ModelAnimStart(visitor_models[0], NE_ANIM_LOOP, 1 << 12);
    NE_ModelSetCoord(visitor_models[0], -1.15f, 0.0f, 0.0f);
    NE_ModelScale(visitor_models[0], 0.65f, 0.65f, 0.65f);

    for (unsigned i = 1; i < SPDS_ROOM_VISITORS; ++i) {
        visitor_models[i] = NE_ModelCreate(NE_Animated);
        if (!visitor_models[i]) {
            NE_End();
            restore_2d_display();
            room_camera = NULL;
            visitor_idle = NULL;
            memset(visitor_models, 0, sizeof(visitor_models));
            return 0;
        }
        NE_ModelClone(visitor_models[i], visitor_models[0]);
        NE_ModelSetCoord(visitor_models[i],
                         i == 1 ? 0.0f : 1.15f, 0.0f, 0.0f);
        NE_ModelScale(visitor_models[i], 0.65f, 0.65f, 0.65f);
    }

    visible_visitors = 0;
    selected_visitor = 0;
    animation_frame = 0;
    renderer_active = 1;
    return 1;
}

void spds_plaza_3d_set_visitors(unsigned count, unsigned selected)
{
    visible_visitors = count > SPDS_ROOM_VISITORS ? SPDS_ROOM_VISITORS : count;
    selected_visitor = selected < visible_visitors ? selected : 0;
}

void spds_plaza_3d_set_visible(unsigned count)
{
    visible_visitors = count > SPDS_ROOM_VISITORS ? SPDS_ROOM_VISITORS : count;
}

void spds_plaza_3d_render(void)
{
    if (!renderer_active)
        return;
    ++animation_frame;
    for (unsigned i = 0; i < visible_visitors; ++i) {
        float base = (float)i * 1.15f - 1.15f;
        unsigned phase = ((animation_frame / 4u) + i * 7u) % 48u;
        int triangle = phase < 24u ? (int)phase : 47 - (int)phase;
        float sway = ((float)triangle - 11.5f) * (0.36f / 23.0f);
        float lift = i == selected_visitor ? 0.09f : 0.0f;
        NE_ModelSetCoord(visitor_models[i], base + sway, lift, 0.0f);
    }
    NE_ModelAnimateAll();
    NE_ProcessArg(draw_room, NULL);
}

void spds_plaza_3d_leave(void)
{
    if (!renderer_active)
        return;
    renderer_active = 0;
    visible_visitors = 0;
    NE_End();
    restore_2d_display();
    room_camera = NULL;
    visitor_idle = NULL;
    memset(visitor_models, 0, sizeof(visitor_models));
}

int spds_plaza_3d_active(void)
{
    return renderer_active;
}
