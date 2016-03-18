#pragma once
#include <new>
#include "MemoryTracker.h"

#ifndef RELEASE_BUILD

#define ENABLE_MEMORY_TRACKER

#endif


#define SAFE_DELETE(pointer) delete pointer; pointer = nullptr;
#define	SAFE_RELEASE(pointer) pointer->Release(); pointer = nullptr;


#ifdef ENABLE_MEMORY_TRACKER

#define GET_RUNTIME Prism::MemoryTracker::GetInstance()->GetRunTime()
#define SET_RUNTIME(value) Prism::MemoryTracker::GetInstance()->SetRunTime(value)
#define RESET_RUNTIME Prism::MemoryTracker::GetInstance()->ResetRunTime()
#define MEMORY_TRACKER_DESTROY Prism::MemoryTracker::Destroy()
#define MEMORY_TRACKER_ALLOW_NEW_DURING_RUNTIME(value) MemoryTracker::GetInstance()->AllowNewDuringRunTime(value)
#define new (Prism::MemoryTracker::GetInstance()->Allocate(__LINE__, __FILE__, __FUNCTION__), false) ? 0 : new

#else

#define GET_RUNTIME false
#define SET_RUNTIME(value)
#define RESET_RUNTIME
#define MEMORY_TRACKER_DESTROY
#define MEMORY_TRACKER_ALLOW_NEW_DURING_RUNTIME(value)

#endif
