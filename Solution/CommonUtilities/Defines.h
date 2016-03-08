#pragma once

#define NUMBER_OF_DIRECTIONAL_LIGHTS 2
#define NUMBER_OF_POINT_LIGHTS 3
#define NUMBER_OF_SPOT_LIGHTS 3
#define MAX_NR_OF_BONES 32
#define OCTREE_WIDTH 10000.f

//If we get random crashes in .exes undefine USE_DGFX
#define USE_DGFX

//Need to rebuild DataExporter for changes to this to take effect
#define DGFX_REMOVE_EXTRA_VERTICES

#ifdef USE_DGFX
//#define CONVERT_TO_DGFX_IN_RUNTIME
#endif

//DO NOT CHANGE
#define DGFX_VERSION 14
#define TERRAIN_VERSION 1

//#define ENABLE_DEBUG_TEXT
#ifndef DLL_EXPORT
//#define THREADED_LOADING
//#define SCENE_USE_OCTREE
#endif

#define USE_BINARY_TERRAIN

//The red text
#define RENDER_DEBUG_TEXT

//Show debug text for portal culling assistance
#define SHOW_PORTAL_CULLING_DEBUG_TEXT

#define USE_LIGHTS