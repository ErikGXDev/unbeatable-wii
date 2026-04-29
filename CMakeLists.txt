cmake_minimum_required(VERSION 3.13)

project(application LANGUAGES C VERSION 1.0.0)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
	source/main.c

  source/beatmaps.c
  
  source/scene_arcade.c
  source/scene_train.c
  
	source/sunlight.c
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

target_include_directories(${PROJECT_NAME} PRIVATE
  include
  /opt/devkitpro/portlibs/wii/include
  /opt/devkitpro/libogc/include
)

file(GLOB DATA_FILES data/*)
if(DATA_FILES)
  dkp_add_embedded_binary_library(application_data ${DATA_FILES})
  target_link_libraries(${PROJECT_NAME} PRIVATE application_data)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
  grrlib
  freetype
  pngu
  png
  jpeg
  z
  bz2
  brotlidec
  brotlicommon
  fat
)

ogc_create_dol(${PROJECT_NAME})

