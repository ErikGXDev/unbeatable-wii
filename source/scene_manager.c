#include <stddef.h>

#include "scene_manager.h"

#include "scene_arcade.h"
#include "scene_train.h"
#include "transition.h"

#include <grrlib.h>

SceneName current_scene = SCENE_ARCADE;
SceneName next_scene = SCENE_ARCADE;

void scene_manager_init(SceneName initial_scene)
{
  current_scene = initial_scene;

  if (current_scene == SCENE_ARCADE)
  {
    arcade_init_scene();
  }
  else if (current_scene == SCENE_TRAIN)
  {
    train_init_scene();
  }
}

void scene_manager_free(SceneName scene)
{
  if (scene == SCENE_ARCADE)
  {
    arcade_free_scene();
  }
  else if (scene == SCENE_TRAIN)
  {
    train_free_scene();
  }
}

void scene_manager_finish_switch()
{
  scene_manager_free(current_scene);

  scene_manager_init(next_scene);

  current_scene = next_scene;

  transition_set_disappear();
}

void scene_manager_start(SceneName initial_scene)
{
  current_scene = initial_scene;

  if (current_scene == SCENE_ARCADE)
  {
    arcade_init_scene();
  }
  else if (current_scene == SCENE_TRAIN)
  {
    train_init_scene();
  }
}

void scene_manager_draw()
{
  if (current_scene == SCENE_ARCADE)
  {
    arcade_draw_scene();
  }
  else if (current_scene == SCENE_TRAIN)
  {
    train_draw_scene();
  }
}

void scene_manager_shutdown()
{
  scene_manager_free(current_scene);
}

void scene_switch_immediate(SceneName target_scene)
{
  if (target_scene == current_scene)
  {
    return;
  }

  next_scene = target_scene;

  scene_manager_finish_switch();
}

void scene_switch(SceneName target_scene)
{
  if (target_scene == current_scene)
  {
    return;
  }

  next_scene = target_scene;

  transition_set_appear_c(scene_manager_finish_switch);
}