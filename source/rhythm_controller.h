#include "beatmaps.h"

extern Beatmap *rhythm_controller_beatmap;

void rhythm_controller_init();

void rhythm_controller_start();
void rhythm_controller_stop();

void rhythm_controller_draw();

void rhythm_controller_free();

float *rhythm_controller_get_cam_pos();