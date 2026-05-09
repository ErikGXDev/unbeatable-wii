/*===========================================
    NoNameNo
    Simple Diffuse light sample code
============================================*/
#include <grrlib.h>

#include <stdlib.h>
#include <math.h>
#include <wiiuse/wpad.h>

#include "generated/exported.h"
#include "draw_3d.h"

#include "scene_train.h"
#include "scene_manager.h"
#include "transition.h"
#include "util.h"

#include "rhythm_controller.h"

#include "enemy_hit_1_png.h"
#include "enemy_hit_2_png.h"

GRRLIB_texImg **texs = NULL;

float map_cam_pos[3];
float map_cam_lookat[3];

void camera_update()
{
    float *new_cam_pos = rhythm_controller_get_cam_pos();
    map_cam_pos[0] = new_cam_pos[0];
    map_cam_pos[1] = new_cam_pos[1];
    map_cam_pos[2] = new_cam_pos[2];

    // Lookat position should just be very far in front of the camera
    map_cam_lookat[0] = map_cam_pos[0];
    map_cam_lookat[1] = map_cam_pos[1];
    map_cam_lookat[2] = map_cam_pos[2] - 10.0f;
}

int train_init_scene()
{

    texs = calloc(exported_texture_count, sizeof(GRRLIB_texImg *));
    if (texs == NULL)
    {
        return 0;
    }

    for (int i = 0; i < exported_texture_count; i++)
    {
        GRRLIB_texImg *tex = GRRLIB_LoadTexture(exported_textures[i]);
        if (!tex)
        {
            continue;
        }
        texs[i] = tex;
    }

    map_cam_pos[0] = TRAINSTATION_MARKER_CAMERA_CENTER_position[0];
    map_cam_pos[1] = TRAINSTATION_MARKER_CAMERA_CENTER_position[1];
    map_cam_pos[2] = TRAINSTATION_MARKER_CAMERA_CENTER_position[2];

    camera_update();

    GRRLIB_SetBackgroundColour(53, 81, 92, 0xFF);

    rhythm_controller_init();
    rhythm_controller_start();

    return 1;
}

int train_draw_scene()
{

    camera_update();

    GRRLIB_Camera3dSettings(UNPACK3(map_cam_pos), 0, 1, 0, UNPACK3(map_cam_lookat));
    GRRLIB_SetLightAmbient(0xDDDDDDDF);

    // Param 4: Enable textures (1)
    // Param 5: Enable normals (1)
    GRRLIB_3dMode(0.1, 1000, 45, 1, 0);

    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS && !transition_in_progress())
    {
        scene_switch_immediate(SCENE_ARCADE);
    }

    GRRLIB_ObjectView(0, 0, 0, 0, 0, 0, 1, 1, 1);

    // Draw all generated OBJs from the Scene header

    int num_objects = exported_num_objects;

    for (int obj_idx = 0; obj_idx < num_objects; obj_idx++)
    {

        int tex_idx = exported_objects_texture_indices[obj_idx];
        if (tex_idx >= 0 && tex_idx < exported_texture_count)
        {
            GRRLIB_SetTexture(texs[tex_idx], 0);
        }
        else
        {
            GRRLIB_SetTexture(NULL, 0); // No texture for this object
        }

        int v_count = exported_objects_vertex_counts[obj_idx];
        const float *v_data = exported_objects_vertices[obj_idx]; // Flat float pointer since it was cast in the header

        GX_Begin(GX_TRIANGLES, GX_VTXFMT0, v_count);

        for (int i = 0; i < v_count; i++)
        {
            const float *pos = &v_data[i * 8 + 0];
            const float *uv = &v_data[i * 8 + 3];
            // const float *normal = &v_data[i * 8 + 5];

            DrawUnlitVertex(pos, uv, 0xFFFFFFFF);

            // DrawSunlitVertex(pos, normal, uv, sun_dir, sun_color, ambient_color);
        }
        GX_End();
    }

    rhythm_controller_draw();

    return 1;
}

void train_free_scene()
{
    if (texs != NULL)
    {
        for (int i = 0; i < exported_texture_count; i++)
        {
            if (texs[i])
            {
                GRRLIB_FreeTexture(texs[i]);
                texs[i] = NULL;
            }
        }
        free(texs);
        texs = NULL;
    }

    rhythm_controller_free();
}
