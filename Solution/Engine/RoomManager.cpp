#include "stdafx.h"
#include "Camera.h"
#include "RoomManager.h"

namespace Prism
{
	RoomManager::RoomManager()
		: myInstances(4096)
	{
	}


	RoomManager::~RoomManager()
	{
	}

	void RoomManager::Add(Instance* anInstance)
	{
		myInstances.Add(anInstance);
	}

	void RoomManager::Remove(Instance* anInstance)
	{
		int index = myInstances.Find(anInstance);
		if (index != myInstances.FoundNone)
		{
			myInstances.RemoveCyclicAtIndex(index);
		}
	}

	const CU::GrowingArray<Instance*>& RoomManager::GetActiveInstances(const Camera& aCamera)
	{
		aCamera;
		return myInstances;
	}
}