#pragma once

#include <stddef.h>

typedef struct HitObject
{
  int lane;
  int time;
  int end_time; // Only for hold notes, otherwise "time"
  int type;     // 1 = hit, 128 = hold
  int side;     // 0 = left, 1 = right
} HitObject;

typedef struct ZoomObject
{
  int time;
  int isZoom;
} ZoomObject;

typedef struct Beatmap
{
  char *artist;
  char *title;
  char *difficulty; // "version:"

  char *audio_filename;
  char *cover_filename;

  int level;
  char *flavor_text;

  HitObject *hit_objects;
  int hit_object_count;

  ZoomObject *zoom_objects;
  int zoom_object_count;

} Beatmap;

Beatmap *load_beatmap(const char *filename);

void free_beatmap(Beatmap *bm);