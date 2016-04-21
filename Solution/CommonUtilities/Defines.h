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
#define DGFX_VERSION 16
#define TERRAIN_VERSION 1
#define NETWORK_VERSION 9

//#define ENABLE_DEBUG_TEXT
#ifndef DLL_EXPORT
#define THREADED_LOADING
//#define SCENE_USE_OCTREE
#endif

#define USE_BINARY_TERRAIN

//The red text
#define RENDER_DEBUG_TEXT

//Show debug text for portal culling assistance
#define SHOW_PORTAL_CULLING_DEBUG_TEXT

#define USE_LIGHT

#define THREAD_PHYSICS

#define NETWORK_UPDATE_INTERVAL 1.f / 30.f

//#define USE_RAW_INPUT

#define SERVER_CONNECT_TO_DEBUGGER false

#define USE_ATMOSPHERE_PARTICLES

#define HEALTH_COLOR { 0.4f, 0.8f, 0.4f, 1.f }

#define UPGRADE_COLOR { 0.7f, 0.7f, 0.2f, 1.f }