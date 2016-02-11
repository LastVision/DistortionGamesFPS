#pragma once
#include <new>
#include "MemoryTracker.h"

//#ifdef ENABLE_MEMORY_TRACKER
//#define new (////Prism::MemoryTracker::GetInstance()->Allocate(__LINE__, __FILE__, __FUNCTION__), false) ? 0 : new
//#endif

#define SAFE_DELETE(pointer) delete pointer; pointer = nullptr;
#define	SAFE_RELEASE(pointer) pointer->Release(); pointer = nullptr;