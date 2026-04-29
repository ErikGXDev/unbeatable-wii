/*===========================================
    NoNameNo
    Simple Diffuse light sample code
============================================*/
#include <grrlib.h>
#include <stdlib.h>

#include <math.h>
#include <wiiuse/wpad.h>


#include "main.h"
#include "scene_arcade.h"

#include "Letter_Gothic_Std_14_Bold_png.h"
/* #include "ground_dev_png.h"  */

/* #include "generated/exported.h"
#include "sunlight.h" */


GRRLIB_texImg *tex_font;

#define UNPACK3(arr) (arr)[0], (arr)[1], (arr)[2]

int main() {

    GRRLIB_Init();
    WPAD_Init();

    tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_InitTileSet(tex_font, 11, 24, 32);


    GRRLIB_Settings.antialias = true;

    arcade_init_scene();


    while(SYS_MainLoop()) {
        GRRLIB_2dMode();
        WPAD_ScanPads();

        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;
    
        

        if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A) {
           
        }

        if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_B) {
           
        }


        if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A) {
            
        }

        if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_B) {
            
        }

        arcade_draw_scene();


        GRRLIB_Render();
    }
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
