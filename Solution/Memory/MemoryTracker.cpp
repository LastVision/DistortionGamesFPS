#include <cstdlib>
#include <DL_Debug.h>
#include "MemoryTracker.h"
#include <sstream>
#include <mutex>


#define MAX_ALLOCATIONS 2500000

void* operator new(size_t aBytes)
{
	void* address = ::malloc(aBytes);
	Prism::MemoryTracker::GetInstance()->Add(address, aBytes, Prism::eMemoryType::NEW);
	return address;
}

void* operator new[](size_t aBytes)
{
	void* address = ::malloc(aBytes);
	Prism::MemoryTracker::GetInstance()->Add(address, aBytes, Prism::eMemoryType::NEW_ARRAY);
	return address;
}

void operator delete(void* aAddress) throw()
{
	Prism::MemoryTracker::GetInstance()->Remove(aAddress);
}

void operator delete[](void* aAddress) throw()
{
	Prism::MemoryTracker::GetInstance()->Remove(aAddress);
}

namespace Prism
{
	MemoryTracker* MemoryTracker::myInstance = nullptr;

	void MemoryTracker::Destroy()
	{
		delete myInstance;
		myInstance = nullptr;
	}

	MemoryTracker* MemoryTracker::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = static_cast<MemoryTracker*>(malloc(sizeof(MemoryTracker)));
			myInstance = new(myInstance)MemoryTracker();

#ifndef RELEASE_BUILD
			myInstance->myMutex = static_cast<std::recursive_mutex*>(malloc(sizeof(std::recursive_mutex)));
			myInstance->myMutex = new(myInstance->myMutex)std::recursive_mutex();
#endif
		}

		return myInstance;
	}

	void MemoryTracker::Allocate(int aLine, const char* aFile, const char* aFunction)
	{

#ifndef RELEASE_BUILD
		if (myMutex != nullptr)
		{
			myMutex->lock();
		}
		if (Prism::MemoryTracker::GetInstance()->myRuntime == true
			&& std::this_thread::get_id() != myAllowThread)
		{
			Prism::MemoryTracker::GetInstance()->myRuntime = false;
			DL_ASSERT_VA("Tried to NEW in runtime");
		}

		myTopicalData.myLine = aLine;
		myTopicalData.myFileName = aFile;
		myTopicalData.myFunctionName = aFunction;

		if (myTopicalData.myFileName == nullptr)
		{
			myTopicalData.myFileName = "Unknown";
		}

		if (myTopicalData.myFunctionName == nullptr)
		{
			myTopicalData.myFunctionName = "Unknown";
		}

		if (myMutex != nullptr)
		{
			myMutex->unlock();
		}
#endif
	}

	void MemoryTracker::Free(void* aAddress)
	{
		Remove(aAddress);
	}

	void MemoryTracker::DumpToFile()
	{

#ifndef RELEASE_BUILD
		DL_PRINT_VA("--- MEMORY LEAKS ---\n");
		DL_PRINT_VA("Bytes:\tLine:\tFile:\t\t\t\t\t\tFunction:");

		for (int i = 0; i < myAllocations; ++i)
		{
			const char* shortPath = strstr(myData[i].myFileName, "solution");
			shortPath += 9;

			DL_PRINT_VA("%i\t\t%i\t\t%s\t\t%s", myData[i].myBytes, myData[i].myLine, shortPath, myData[i].myFunctionName);
		}

		const char* shortPath = strstr(myData[0].myFileName, "solution");
		shortPath += 9;

		DL_ASSERT_VA("\nMEMORYLEAK!\nFile: %s\nFunction: %s\nLine: %i\n\nTotal Leaks: %i"
			, shortPath, myData[0].myFunctionName, myData[0].myLine, myAllocations);
		
#endif
	}

	void MemoryTracker::Add(void* aAddress, size_t aBytes, eMemoryType aMemoryType)
	{
#ifndef RELEASE_BUILD
		if (myMutex != nullptr)
		{
			myMutex->lock();
		}
		if (myTopicalData.myLine < 1)
		{
			myTopicalData.myFileName = "";
			myTopicalData.myFunctionName = "";
			myTopicalData.myLine = -1;
			myTopicalData.myType = eMemoryType::UNKNOWN;

			if (myMutex != nullptr)
			{
				myMutex->unlock();
			}
			return;
		}

		myMemoryStatistics.myAccumulatedMemoryAllocated += aBytes;
		myMemoryStatistics.myCurrentMemoryAllocated += aBytes;
		++myMemoryStatistics.myAccumulatedNumberOfAllocations;

		myTopicalData.myAddress = aAddress;
		myTopicalData.myBytes = aBytes;
		myTopicalData.myType = aMemoryType;

		++myAllocations;
		DL_ASSERT_EXP(myAllocations <= MAX_ALLOCATIONS, "Too many memory allocations, need to increase MAX_ALLOCATIONS in MemoryTracker.cpp?");
		
		myData[myAllocations - 1] = myTopicalData;

		myTopicalData.myFileName = "";
		myTopicalData.myFunctionName = "";
		myTopicalData.myLine = -1;
		myTopicalData.myType = eMemoryType::UNKNOWN;

		if (myMutex != nullptr)
		{
			myMutex->unlock();
		}
#endif
	}

	void MemoryTracker::Remove(void* aAddress, bool aLock)
	{
#ifndef RELEASE_BUILD
		if (aLock == true && myMutex != nullptr)
		{
			myMutex->lock();
		}

		for (int i = 0; i < myAllocations; ++i)
		{
			if (myData[i].myAddress == aAddress)
			{
				--myAllocations;
				myMemoryStatistics.myCurrentMemoryAllocated -= myData[i].myBytes;
				myData[i] = myData[myAllocations];
				break;
			}
		}

		if (aLock == true && myMutex != nullptr)
		{
			myMutex->unlock();
		}
#endif
	}

	MemoryTracker::MemoryTracker()
#ifndef RELEASE_BUILD
		: myData(nullptr)
		, myAllocations(0)
		, myRuntime(false)
#endif
	{
#ifndef RELEASE_BUILD
		myData = reinterpret_cast<MemoryData*>(::malloc(sizeof(MemoryData) * MAX_ALLOCATIONS));
#endif
	}


	MemoryTracker::~MemoryTracker()
	{
#ifndef RELEASE_BUILD
		if (myAllocations > 0)
		{
			DumpToFile();
			::free(myData);
		}

		::free(myMutex);
		myMutex = nullptr;
#endif
	}
}