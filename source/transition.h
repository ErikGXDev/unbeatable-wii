#pragma once

void transition_set_appear();
void transition_set_appear_c(void (*callback)());

void transition_set_disappear();
void transition_set_disappear_c(void (*callback)());

void transition_update();

int transition_in_progress();
int transition_is_covered();

int transition_get_progress_percent();
