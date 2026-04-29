
/* 
"name": "Default",
"foreground": "rgb(255, 75, 125)",
"background": "rgb(249, 247, 213)",
"decoration": "rgb(224, 222, 191)",
"muted": "rgb(180, 179, 153)",
"accent": "rgb(0, 0, 0)" */
 /* {
    "name": "Default",
    "foreground": "#FF4B7DFF",
    "background": "#F9F7D5FF",
    "decoration": "#E0DEBFFF",
    "muted": "#B4B399FF",
    "accent": "#000000FF"
  }, */
#include <grrlib.h>

#include <math.h>
#include <wiiuse/wpad.h>

#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Letter_Gothic_Std_14_Bold_png.h"

#include "main.h"
#include "beatmaps.h"

#define THEME_BG 249, 247, 213, 0xFF
#define THEME_FG 255, 75, 125, 0xFF
#define THEME_DECO 224, 222, 191, 0xFF
#define THEME_MUTED 180, 179, 153, 0xFF
#define THEME_ACCENT 0, 0, 0, 0xFF


Beatmap *current_beatmap = NULL;

int arcade_init_scene() {
    if (!fatInitDefault()) {
      goto error;
    }

    current_beatmap = load_beatmap("/beatmaps/star.txt");

    return 1;


    error:
      exit(1);
      return 0;
}

int arcade_draw_scene() {
    GRRLIB_2dMode();

    GRRLIB_SetBackgroundColour(THEME_BG);

    GRRLIB_Printf(10, 10, tex_font, 0xFF4B7DFF, 1, "Hello Arcade!");

    

    if (current_beatmap) {
      GRRLIB_Printf(10, 30, tex_font, 0xFF4B7DFF, 1, "Loaded beatmap: %s - %s [%s]", current_beatmap->artist, current_beatmap->title, current_beatmap->difficulty);
      GRRLIB_Printf(10, 50, tex_font, 0xFF4B7DFF, 1, "Memory Size: %d KB", (current_beatmap->hit_object_count * sizeof(HitObject)) / 1024);
      GRRLIB_Printf(10, 70, tex_font, 0xFF4B7DFF, 1, "Audio file: %s", current_beatmap->audio_filename);
      GRRLIB_Printf(10, 90, tex_font, 0xFF4B7DFF, 1, "Hit objects: %d", current_beatmap->hit_object_count);

      for (int i = 0; i < current_beatmap->hit_object_count && i < 16; i++) {
        HitObject *ho = &current_beatmap->hit_objects[i];
        GRRLIB_Printf(10, 110 + i*20, tex_font, 0xFF4B7DFF, 1, "HitObject %d: lane=%d time=%d type=%d", i, ho->lane, ho->time, ho->type);
      }
    } else {
      GRRLIB_Printf(10, 30, tex_font, 0xFF4B7DFF, 1, "Failed to load beatmap :(");
    }


    


    return 1;

}