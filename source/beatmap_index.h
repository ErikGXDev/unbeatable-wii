

#include "beatmaps.h"
#include <grrlib.h>

extern Beatmap *beatmap_index[128];

extern int beatmap_count;

void free_beatmap_index();
int load_beatmap_index();

GRRLIB_texImg *get_beatmap_cover(int index);