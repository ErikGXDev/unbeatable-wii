#pragma once

typedef enum SceneName
{
  SCENE_ARCADE,
  SCENE_TRAIN,
} SceneName;

void scene_manager_start(SceneName initial_scene);
void scene_manager_shutdown();
void scene_manager_draw();
void scene_switch_immediate(SceneName next_scene);
void scene_switch(SceneName next_scene);