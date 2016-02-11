#pragma once

//#define USE_VTUNE_API

#ifdef USE_VTUNE_API

#include <ittnotify.h>
#pragma comment(lib, "libittnotify.lib")


static __itt_domain* domain = __itt_domain_create("MyTraces.MyDomain");

enum class VTUNE
{
	RENDER,
	UPDATE,
	FUNCTION_TIMERS,
	GAME_RENDER,
};

static __itt_string_handle* __VTUNE_STRING_HANDLES__[] =
{
	__itt_string_handle_create(TEXT("Render")),
	__itt_string_handle_create(TEXT("UPDATE")),
	__itt_string_handle_create(TEXT("RenderFunctionTimers")),
	__itt_string_handle_create(TEXT("Game::Render")),
};

#define VTUNE_EVENT_BEGIN(VTUNE_ENUM) __itt_task_begin(domain, __itt_null, __itt_null, __VTUNE_STRING_HANDLES__[static_cast<int>(VTUNE_ENUM)]);
#define VTUNE_EVENT_END() __itt_task_end(domain);

#define VTUNE_PAUSE __itt_pause();
#define VTUNE_RESUME __itt_resume();

#else
#define VTUNE_EVENT_BEGIN(TASKNAME)
#define VTUNE_EVENT_END()

#define VTUNE_PAUSE
#define VTUNE_RESUME
#endif