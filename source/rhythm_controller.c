#include <stdio.h>
#include <asndlib.h>
#include <fat.h>
#include "ogg/oggplayer.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grrlib.h>
#include "main.h"
#include <time.h>
#include "beatmaps.h"
#include <gccore.h>

#include "generated/exported.h"

#include "util.h"

#include "draw_3d.h"

#include "enemy_hit_1_png.h"
#include "enemy_hit_2_png.h"
#include "circle_png.h"

#define RHYTHM_CONTROLLER_START_DELAY_MS 2000
#define RHYTHM_CONTROLLER_WORLD_SPAN 10 // how many units away the notes can appear (x-coord, away from marker)

Beatmap *rhythm_controller_beatmap = NULL;
void *rhythm_controller_music_buffer = NULL;
long rhythm_controller_music_size = 0;

int rhythm_controller_music_started = 0;

s64 rhythm_controller_start_time = 0;

float rhythm_controller_cam_pos[3];
float rhythm_controller_cam_target_pos[3];

int rhythm_controller_current_side = 1; // 0 = left, 1 = right
int rhythm_controller_current_zoom = 0; // 0 = normal, 1 = zoom

s64 rhythm_controller_get_elapsed_time()
{
  return (s64)getCurrentTimeMs() - rhythm_controller_start_time - RHYTHM_CONTROLLER_START_DELAY_MS;
}

GRRLIB_texImg *enemy_hit_1 = NULL;
GRRLIB_texImg *enemy_hit_2 = NULL;
GRRLIB_texImg *hit_circle = NULL;

void rhythm_controller_init()
{
  enemy_hit_1 = GRRLIB_LoadTexture(enemy_hit_1_png);
  enemy_hit_2 = GRRLIB_LoadTexture(enemy_hit_2_png);
  hit_circle = GRRLIB_LoadTexture(circle_png);

  if (rhythm_controller_beatmap && rhythm_controller_beatmap->audio_filename)
  {
    FILE *fp = fopen(rhythm_controller_beatmap->audio_filename, "rb");
    if (fp)
    {
      fseek(fp, 0, SEEK_END);
      long size = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      rhythm_controller_music_size = size;

      rhythm_controller_music_buffer = malloc(size);

      if (rhythm_controller_music_buffer)
      {
        fread(rhythm_controller_music_buffer, 1, size, fp);
      }
      fclose(fp);
    }
  }

  rhythm_controller_music_started = 0;

  ASSIGN3(rhythm_controller_cam_pos, TRAINSTATION_MARKER_CAMERA_RIGHT_position);

  ASSIGN3(rhythm_controller_cam_target_pos, TRAINSTATION_MARKER_CAMERA_RIGHT_position);
}

void rhythm_controller_start()
{
  // PlayOgg(rhythm_controller_music_buffer, rhythm_controller_music_size, 0, OGG_ONE_TIME);
  rhythm_controller_start_time = getCurrentTimeMs();
}

void rhythm_controller_stop()
{
  StopOgg();
  rhythm_controller_start_time = 0;
}

void rhythm_controller_free()
{
  StopOgg();
  if (rhythm_controller_music_buffer)
  {
    free(rhythm_controller_music_buffer);
    rhythm_controller_music_buffer = NULL;
    rhythm_controller_music_size = 0;
  }

  GRRLIB_FreeTexture(enemy_hit_1);
  GRRLIB_FreeTexture(enemy_hit_2);
}

#define RHYTHM_CONTROLLER_RIGHT_X 300
#define RHYTHM_CONTROLLER_VIEW_WIDTH_MS 1500

void rhythm_controller_draw()
{
  if (!rhythm_controller_beatmap)
  {
    return;
  }

  s64 current_time = rhythm_controller_get_elapsed_time();

  if (current_time >= 0 && !rhythm_controller_music_started)
  {
    PlayOgg(rhythm_controller_music_buffer, rhythm_controller_music_size, 0, OGG_ONE_TIME);
    rhythm_controller_music_started = 1;
  }

  // GRRLIB_Rectangle(RHYTHM_CONTROLLER_RIGHT_X - 5, 0, 10, 240, 0xFFFFFFFF, true);

  DrawSquare(TRAINSTATION_MARKER_LEFT_BOTTOM_position, hit_circle, 0xFFFFFFFF, 0.1f, 0.1f);
  DrawSquare(TRAINSTATION_MARKER_LEFT_TOP_position, hit_circle, 0xFFFFFFFF, 0.1f, 0.1f);
  DrawSquare(TRAINSTATION_MARKER_RIGHT_BOTTOM_position, hit_circle, 0xFFFFFFFF, 0.1f, 0.1f);
  DrawSquare(TRAINSTATION_MARKER_RIGHT_TOP_position, hit_circle, 0xFFFFFFFF, 0.1f, 0.1f);

  // MARK: Camera Updates
  float *new_camera_pos = lerp_vec3(rhythm_controller_cam_pos, rhythm_controller_cam_target_pos, 0.1f);
  ASSIGN3(rhythm_controller_cam_pos, new_camera_pos);

  // Determine zoom state from zoom objects. Use parity of passed zoom triggers as base state,
  // and allow upcoming non-zoom flip to temporarily indicate by zooming.
  int passed_zoom_count = 0;
  int passed_flips = 0;
  int upcoming_normal_flip = 0;
  for (int i = 0; i < rhythm_controller_beatmap->zoom_object_count; i++)
  {
    ZoomObject *obj = &rhythm_controller_beatmap->zoom_objects[i];
    s64 time_diff = (s64)obj->time - (s64)current_time;

    if (obj->isZoom && time_diff < 0)
    {
      passed_zoom_count++;
    }

    if (!obj->isZoom && time_diff < 0)
    {
      passed_flips++;
    }

    // Add a small indicator for upcoming flip change by zooming temporarily.
    if (!obj->isZoom && time_diff > 0 && time_diff < 750)
    {
      upcoming_normal_flip = 1;
    }
  }

  rhythm_controller_current_zoom = passed_zoom_count % 2;
  if (upcoming_normal_flip)
  {
    rhythm_controller_current_zoom = 1;
  }

  rhythm_controller_current_side = 1 - (passed_flips % 2);

  if (rhythm_controller_current_zoom)
  {
    ASSIGN3(rhythm_controller_cam_target_pos, TRAINSTATION_MARKER_CAMERA_CENTER_position);
  }
  else
  {
    if (rhythm_controller_current_side == 0)
    {
      ASSIGN3(rhythm_controller_cam_target_pos, TRAINSTATION_MARKER_CAMERA_LEFT_position);
    }
    else
    {
      ASSIGN3(rhythm_controller_cam_target_pos, TRAINSTATION_MARKER_CAMERA_RIGHT_position);
    }
  }

  // MARK: Note Rendering

  for (int i = 0; i < rhythm_controller_beatmap->hit_object_count; i++)
  {
    HitObject *obj = &rhythm_controller_beatmap->hit_objects[i];
    s64 time_diff = (s64)obj->time - (s64)current_time;

    if (time_diff < -40)
    {
      continue; // Skip notes that are too old
    }

    if (time_diff > RHYTHM_CONTROLLER_VIEW_WIDTH_MS + 1000)
    {
      break; // No need to check further, as notes are sorted by time
    }

    // Only lane 3 and 4 should be visible.
    if (obj->lane != 3 && obj->lane != 4)
    {
      continue;
    }

    // Actual 3d calculation
    int side = obj->side; // 0 = left, 1 = right
    int multiplier = side == 0 ? -1 : 1;

    // span from marker to the right max
    // topMarker and bottomMarker share the same x, so top can be used for both
    float marker_x = side == 0 ? TRAINSTATION_MARKER_LEFT_TOP_position[0] : TRAINSTATION_MARKER_RIGHT_TOP_position[0];
    float span = marker_x + RHYTHM_CONTROLLER_WORLD_SPAN * multiplier;

    // Keep signed timing so notes continue past the marker after their hit time.
    float t = (float)time_diff / (float)RHYTHM_CONTROLLER_VIEW_WIDTH_MS;

    // Map x position
    float x = marker_x + (span - marker_x) * t;

    // Choose the correct top/bottom markers depending on side (left/right)
    float y;
    float z;
    if (side == 0)
    {
      y = (obj->lane == 3) ? TRAINSTATION_MARKER_LEFT_TOP_position[1] : TRAINSTATION_MARKER_LEFT_BOTTOM_position[1];
      z = TRAINSTATION_MARKER_LEFT_TOP_position[2];
    }
    else
    {
      y = (obj->lane == 3) ? TRAINSTATION_MARKER_RIGHT_TOP_position[1] : TRAINSTATION_MARKER_RIGHT_BOTTOM_position[1];
      z = TRAINSTATION_MARKER_RIGHT_TOP_position[2];
    }

    // switch animation between 0 and 1
    int anim_frame = ((int)(time_diff / 100)) & 1;

    float scale = 0.1f;

    DrawSquare((float[]){x, y, z}, anim_frame == 0 ? enemy_hit_1 : enemy_hit_2, 0xFFFFFFFF, scale * multiplier, scale);
  }

  // Debug overlay text should be drawn in 2D mode.
  GRRLIB_2dMode();

  GRRLIB_Rectangle(0, 0, GRRLIB_ScreenWidth, 40, 0x000000FF, true);
  GRRLIB_Rectangle(0, GRRLIB_ScreenHeight - 40, GRRLIB_ScreenWidth, 40, 0x000000FF, true);

  GRRLIB_Printf(30, 12, font_arcade_12, 0xFFFFFFFF, 1, "Time: %lld ms", current_time);

  // print info about zoom and hitobjects for debugging
  GRRLIB_Printf(30, 24, font_arcade_12, 0xFFFFFFFF, 1, "Zoom: %d", rhythm_controller_current_zoom);
  GRRLIB_Printf(30, 36, font_arcade_12, 0xFFFFFFFF, 1, "Side: %d", rhythm_controller_current_side);
  GRRLIB_Printf(30, 48, font_arcade_12, 0xFFFFFFFF, 1, "Zoom count: %d", rhythm_controller_beatmap->zoom_object_count);
  GRRLIB_Printf(30, 60, font_arcade_12, 0xFFFFFFFF, 1, "HitObject count: %d", rhythm_controller_beatmap->hit_object_count);
}

float *rhythm_controller_get_cam_pos()
{
  return rhythm_controller_cam_pos;
}