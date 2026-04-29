// Beatmap parser, soon in a github repo near you!

#pragma once

#include <stddef.h>

typedef struct HitObject {
  int lane;
  int time;
  int end_time; // Only for hold notes, otherwise "time"
  int type; // 1 = hit, 128 = hold
} HitObject;

typedef struct Beatmap {
  char *artist;
  char *title;
  char *difficulty; // "version:"

  char *audio_filename;
  char *cover_filename;

  HitObject *hit_objects;
  int hit_object_count;
} Beatmap;

Beatmap *load_beatmap(const char *filename);

void free_beatmap(Beatmap *bm);