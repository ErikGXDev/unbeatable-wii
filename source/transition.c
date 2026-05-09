#include <stdio.h>
#include <grrlib.h>
#include "main.h"

#define TRANSITION_DURATION 60
#define RECT_COUNT 20

int transition_direction = 0; // 0 = disappearing, 1 = appearing

void (*transition_callback)();

void transition_set_appear()
{
  transition_direction = 1;
}

void transition_set_appear_c(void (*callback)())
{
  transition_callback = callback;
  transition_set_appear();
}

void transition_set_disappear()
{
  transition_direction = 0;
}

void transition_set_disappear_c(void (*callback)())
{
  transition_callback = callback;
  transition_set_disappear();
}

int transition_progress = 0; // in frames

int transition_in_progress()
{
  return transition_direction == 1 || transition_progress > 0;
}

int transition_is_covered()
{
  return transition_direction == 1 && transition_progress >= TRANSITION_DURATION;
}

int transition_get_progress_percent()
{
  return (transition_progress * 100) / TRANSITION_DURATION;
}

void transition_update()
{

  if (transition_direction == 1)
  {
    if (transition_progress >= TRANSITION_DURATION && transition_callback)
    {
      transition_callback();

      transition_callback = NULL;
    }
    if (transition_progress < TRANSITION_DURATION)
    {
      transition_progress++;
    }
  }
  else
  {
    if (transition_progress <= 0 && transition_callback)
    {
      transition_callback();

      transition_callback = NULL;
    }
    if (transition_progress > 0)
    {
      transition_progress--;
    }
  }

  if (transition_progress == 0)
  {
    return; // No need to draw anything if transition is fully disappeared
  }

  GRRLIB_2dMode();

  // Draw rectangles
  float column_width = (float)GRRLIB_ScreenWidth / RECT_COUNT;

  for (int i = 0; i < RECT_COUNT; i++)
  {
    int stagger_offset = i * (TRANSITION_DURATION / 2) / RECT_COUNT;
    int local_progress = transition_progress - stagger_offset;

    int local_duration = TRANSITION_DURATION / 2;
    if (local_progress < 0)
      local_progress = 0;
    if (local_progress > local_duration)
      local_progress = local_duration;

    float height_percent = (float)local_progress / local_duration;
    float rect_height = rmode->efbHeight * height_percent;

    GRRLIB_Rectangle(i * column_width, 0, column_width + 1.0f, rect_height, 0x000000FF, true);
  }
}
