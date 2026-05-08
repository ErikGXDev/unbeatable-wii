/*===========================================
    NoNameNo
    Simple Diffuse light sample code
============================================*/
#include <grrlib.h>

#include <stdlib.h>
#include <math.h>
#include <wiiuse/wpad.h>

#include "generated/exported.h"
#include "sunlight.h"

#define UNPACK3(arr) (arr)[0], (arr)[1], (arr)[2]

int main2()
{

    GRRLIB_Init();
    WPAD_Init();

    GRRLIB_texImg *texs[exported_texture_count];

    int failed = 0;

    for (int i = 0; i < exported_texture_count; i++)
    {
        GRRLIB_texImg *tex = GRRLIB_LoadTexture(exported_textures[i]);
        if (!tex)
        {
            failed++;
            continue;
        }
        texs[i] = tex;
    }

    GRRLIB_Settings.antialias = true;

    GRRLIB_SetBackgroundColour(53, 81, 92, 0xFF);

    float cam_pos[3] = {UNPACK3(TRAINSTATION_MARKER_CAMERA_CENTER_position)};

    while (SYS_MainLoop())
    {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
            break;

        GRRLIB_Camera3dSettings(cam_pos[0], cam_pos[1], cam_pos[2], 0, 1, 0, UNPACK3(TRAINSTATION_MARKER_LOOKAT_position));
        GRRLIB_SetLightAmbient(0xDDDDDDDF);

        // Param 4: Enable textures (1)
        // Param 5: Enable normals (1)
        GRRLIB_3dMode(0.1, 1000, 45, 1, 1);

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
        {
            cam_pos[0] += 0.5f;
        }

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_B)
        {
            cam_pos[0] -= 0.5f;
        }

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
        {
        }

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_B)
        {
        }

        GRRLIB_ObjectView(0, 0, 0, 0, 0, 0, 1, 1, 1);

        // GRRLIB_SetTexture(tex_mat, 0);

        // Draw all generated OBJs from the Scene header
        int num_objects = exported_num_objects;

        float sun_dir[3] = {1.0f, 1.0f, 0.5f};
        unsigned int sun_color = 0xFFFFFFFF;
        unsigned int ambient_color = 0xBBBBBBFF;

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
                const float *normal = &v_data[i * 8 + 5];
                DrawSunlitVertex(pos, normal, uv, sun_dir, sun_color, ambient_color);
            }
            GX_End();
        }

        /* GRRLIB_ObjectView(0.0f, 0.0f, -1.0f, 0, 0, 0, 1.0f, 1.0f, 1.0f);

        GRRLIB_DrawCube(0.8f, 0, 0xFF0000FF); */

        // Switch to 2D Mode to display text
        GRRLIB_2dMode();

        GRRLIB_Render();
    }
    // GRRLIB_FreeTexture(tex_mat);

    for (int i = 0; i < exported_texture_count; i++)
    {
        GRRLIB_FreeTexture(texs[i]);
    }

    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
