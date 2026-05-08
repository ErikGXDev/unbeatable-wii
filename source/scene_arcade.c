
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
#include <gccore.h>
#include <malloc.h>
#include <asndlib.h>
// #include <mp3player.h>
#include "ogg/oggplayer.h"

#include "main.h"
#include "beatmaps.h"

#include "synth_mp3.h"

#include "beatmap_index.h"

#include "util.h"

#define THEME_BG 249, 247, 213, 0xFF

#define COLOR_FG 0xFF4B7DFF
#define COLOR_BG 0xF9F7D5FF
#define COLOR_DECO 0xE0DEBFFF
#define COLOR_MUTED 0xB4B399FF
#define COLOR_ACCENT 0x000000FF

#define COLOR_ALPHA(color, alpha) (((color) & 0xFFFFFF00) | ((alpha) & 0xFF))

int selected_beatmap = 0;
GRRLIB_texImg *current_cover = NULL;
void *current_ogg_buffer = NULL;
int audio_load_timer = -1;

// Update cover and audio when selected_beatmap changes
void arcade_update_slot()
{
  if (current_cover)
  {
    GRRLIB_FreeTexture(current_cover);
    current_cover = NULL;
  }

  if (beatmap_count > 0)
  {
    current_cover = get_beatmap_cover(selected_beatmap);
  }
}

void arcade_update_audio()
{
  Beatmap *bm = beatmap_index[selected_beatmap];

  if (bm && bm->audio_filename)
  {
    FILE *fp = fopen(bm->audio_filename, "rb");
    if (fp)
    {
      fseek(fp, 0, SEEK_END);
      long size = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      current_ogg_buffer = malloc(size);

      if (current_ogg_buffer)
      {
        fread(current_ogg_buffer, 1, size, fp);

        PlayOgg(current_ogg_buffer, size, 0, OGG_ONE_TIME);
      }
      fclose(fp);
    }
  }
}

int arcade_init_scene()
{
  if (!fatInitDefault())
  {
    goto error;
  }

  load_beatmap_index();

  arcade_update_slot();
  audio_load_timer = 30; // Load initial audio after half a second

  return 1;

error:
  exit(1);
  return 0;
}

int arcade_draw_carousel()
{

  int center = 210;
  int space = 50;
  // int font_size = 20;
  int rect_size = 35;

  for (int i = -2; i <= 3; i++)
  {
    int index = (selected_beatmap + i + beatmap_count) % beatmap_count;

    Beatmap *bm = beatmap_index[index];

    int y = center + i * space;

    char display[64];
    snprintf(display, sizeof(display), "// %s", bm->title);

    u32 color = COLOR_FG;

    GRRLIB_Printf(56, y - 13, font_arcade_12, 0.83, color, "LV");
    GRRLIB_Printf(26, y, font_rubik_24, 1, COLOR_FG, "10", bm->level);

    if (i == 0)
    {
      color = COLOR_BG;

      int display_len = strlen(display);
      int text_width = display_len * 16 + 32; // Approximation, since

      GRRLIB_Rectangle(75, y - rect_size / 2, text_width, rect_size, COLOR_ACCENT, true);
    }

    GRRLIB_Printf(85, y - 14, font_arcade_20, color, 1, display);
    // GRRLIB_PrintfTTF(85, y - font_size / 2, tex_font_arcade, bm->audio_filename, font_size, color);
  }

  return 1;
}

int arcade_draw_scene()
{
  GRRLIB_2dMode();

  if (WPAD_ButtonsDown(0) & WPAD_BUTTON_UP)
  {
    selected_beatmap--;
    if (selected_beatmap < 0)
    {
      selected_beatmap += beatmap_count;
    }
    StopOgg();
    if (current_ogg_buffer)
    {
      free(current_ogg_buffer);
      current_ogg_buffer = NULL;
    }
    arcade_update_slot();
    audio_load_timer = 30; // Wait 30 frames (0.5s) before loading audio
  }

  if (WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN)
  {
    selected_beatmap++;
    selected_beatmap %= beatmap_count;
    StopOgg();
    if (current_ogg_buffer)
    {
      free(current_ogg_buffer);
      current_ogg_buffer = NULL;
    }
    arcade_update_slot();
    audio_load_timer = 10;
  }

  if (audio_load_timer > 0)
  {
    audio_load_timer--;
  }
  else if (audio_load_timer == 0)
  {
    arcade_update_audio();
    audio_load_timer = -1;
  }

  GRRLIB_SetBackgroundColour(THEME_BG);

  GRRLIB_Printf(420, -40, font_rubik_64, COLOR_ALPHA(COLOR_ACCENT, 20), 2, "SONG");
  GRRLIB_Printf(20, 400, font_rubik_64, COLOR_ALPHA(COLOR_ACCENT, 20), 2, "SELECT");

  GRRLIB_Printf(30, 10, font_arcade_24, COLOR_FG, 1, "arcade.");

  GRRLIB_Printf(30, 46, font_arcade_12, COLOR_FG, 1, "beatmaps: %d", beatmap_count);

  if (beatmap_count > 0)
  {
    arcade_draw_carousel();

    if (current_cover)
    {
      GRRLIB_DrawImg(550, 80, current_cover, 0, 250.0f / current_cover->w, 250.0f / current_cover->h, 0xFFFFFFFF);
    }
  }

  return 1;
}
