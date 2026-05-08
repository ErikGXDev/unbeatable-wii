/*===========================================
    NoNameNo
    Simple Diffuse light sample code
============================================*/
#include <grrlib.h>
#include <stdlib.h>

#include <math.h>

#include <stdlib.h>
#include <gccore.h>
#include <asndlib.h>
#include <mp3player.h>

#include <wiiuse/wpad.h>

#include "main.h"
#include "scene_arcade.h"

#include "arcade_12_12x20_png.h"
#include "arcade_20_16x28_png.h"
#include "arcade_24_18x36_png.h"

#include "rubik_24_24x28_png.h"
#include "rubik_64_56x64_png.h"
/* #include "ground_dev_png.h"  */

/* #include "generated/exported.h"
#include "sunlight.h" */

GRRLIB_texImg *font_rubik_24;
GRRLIB_texImg *font_rubik_64;

GRRLIB_texImg *font_arcade_12;
GRRLIB_texImg *font_arcade_20;
GRRLIB_texImg *font_arcade_24;

#define UNPACK3(arr) (arr)[0], (arr)[1], (arr)[2]

int main()
{

    GRRLIB_Init();
    WPAD_Init();

    ASND_Init();
    MP3Player_Init();

    font_arcade_12 = GRRLIB_LoadTexture(arcade_12_12x20_png);
    GRRLIB_InitTileSet(font_arcade_12, 12, 20, 32);
    font_arcade_20 = GRRLIB_LoadTexture(arcade_20_16x28_png);
    GRRLIB_InitTileSet(font_arcade_20, 16, 28, 32);
    font_arcade_24 = GRRLIB_LoadTexture(arcade_24_18x36_png);
    GRRLIB_InitTileSet(font_arcade_24, 18, 36, 32);

    font_rubik_24 = GRRLIB_LoadTexture(rubik_24_24x28_png);
    GRRLIB_InitTileSet(font_rubik_24, 24, 28, 32);
    font_rubik_64 = GRRLIB_LoadTexture(rubik_64_56x64_png);
    GRRLIB_InitTileSet(font_rubik_64, 56, 64, 32);

    GRRLIB_Settings.antialias = true;

    arcade_init_scene();

    while (SYS_MainLoop())
    {
        GRRLIB_2dMode();
        WPAD_ScanPads();

        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
            break;

        /* if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
        {
        }

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_B)
        {
        }

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
        {
        }

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_B)
        {
        }
        */
        arcade_draw_scene();

        GRRLIB_Render();
    }

    GRRLIB_FreeTexture(font_arcade_12);
    GRRLIB_FreeTexture(font_arcade_20);
    GRRLIB_FreeTexture(font_arcade_24);
    GRRLIB_FreeTexture(font_rubik_24);
    GRRLIB_FreeTexture(font_rubik_64);

    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
