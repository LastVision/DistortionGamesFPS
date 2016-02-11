#include "stdafx.h"
#include "StreakDataContainer.h"
#include "StreakEmitterData.h"

#ifdef DLL_EXPORT
#include "Engine.h"
#include "FileWatcher.h"
#endif

namespace Prism
{
	StreakDataContainer* StreakDataContainer::myInstance = nullptr;

	StreakDataContainer* StreakDataContainer::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new StreakDataContainer();
		}

		return myInstance;
	}

	void StreakDataContainer::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	StreakDataContainer::~StreakDataContainer()
	{
		for (auto it = myParticleData.begin(); it != myParticleData.end(); ++it)
		{
			delete it->second;
			it->second = nullptr;
		}
	}

	StreakEmitterData* StreakDataContainer::GetStreakData(const char* aFilePath)
	{
#ifndef DLL_EXPORT
		auto it = myParticleData.find(aFilePath);

		if (it == myParticleData.end())
		{
			LoadParticleData(aFilePath);
		}

		return myParticleData[aFilePath];
#else
		auto it = myParticleData.find(aFilePath);

		if (it != myParticleData.end()) 
		{
			delete it->second;
			it->second = nullptr;
			myParticleData.erase(aFilePath);
		}
		LoadParticleData(aFilePath);

		return myParticleData[aFilePath];
#endif
	}

	void StreakDataContainer::LoadParticleData(const char* aFilePath)
	{
		StreakEmitterData* newData = new StreakEmitterData();

		newData->LoadDataFile(aFilePath);
		DL_ASSERT_EXP(newData != nullptr, "Failed to load data. newData became nullptr.");

		myParticleData[aFilePath] = newData;

	}
}

 