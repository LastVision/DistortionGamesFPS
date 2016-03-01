#include "stdafx.h"
#include "Camera.h"
#include "Instance.h"
#include <Intersection.h>
#include "Portal.h"
#include "Room.h"
#include "RoomManager.h"

namespace Prism
{
	RoomManager::RoomManager()
		: myRooms(128)
		, myInstances(4096)
		, myAlwaysRenderInstances(128)
		, myActiveInstances(4096)
	{
	}


	RoomManager::~RoomManager()
	{
		myActiveInstances.RemoveAll();
		myInstances.RemoveAll();
	}

	void RoomManager::Add(Room* aRoom)
	{
		myRooms.Add(aRoom);
	}

	void RoomManager::CalcPortals()
	{
		for (int i = 0; i < myRooms.Size(); ++i)
		{
			for (int j = i + 1; j < myRooms.Size(); ++j)
			{
				if (myRooms[i]->Collide(*myRooms[j]) == true)
				{
					myPortals.Add(new Portal(myRooms[i], myRooms[j]));
				}
			}
		}
	}

	void RoomManager::Add(Instance* anInstance, bool anAlwaysRender)
	{
		if (anAlwaysRender == true)
		{
			myAlwaysRenderInstances.Add(anInstance);
		}
		else
		{
			bool success(false);
			for (int i = 0; i < myRooms.Size(); ++i)
			{
				if (myRooms[i]->Inside(anInstance->GetPosition()) == true)
				{
					myInstances.Add(InstanceInRoom(i, anInstance));
					success = true;
					break;
				}
			}

			//DL_ASSERT_EXP(success == true, "Instance found outside room.");
		}
	}

	void RoomManager::Remove(Instance* anInstance)
	{
		for (int i = 0; i < myInstances.Size(); ++i)
		{
			if (myInstances[i].myInstance == anInstance)
			{
				myInstances.RemoveCyclicAtIndex(i);
				return;
			}
		}

		//DL_ASSERT("Unable to remove instance that does not exist in Room Manager.");
	}

	const CU::GrowingArray<Instance*>& RoomManager::GetActiveInstances(const Camera& aCamera)
	{
		int currentRoom = GetRoomId(aCamera.GetOrientation().GetPos());

		myActiveInstances.RemoveAll();

		for (int i = 0; i < myAlwaysRenderInstances.Size(); ++i)
		{
			myActiveInstances.Add(myAlwaysRenderInstances[i]);
		}
		for (int i = 0; i < myInstances.Size(); ++i)
		{
			if (currentRoom == myInstances[i].myRoomId)
			{
				myActiveInstances.Add(myInstances[i].myInstance);
			}
		}
		return myActiveInstances;
	}

	int RoomManager::GetRoomId(const CU::Vector3<float>& aPosition) const
	{
		for (int i = 0; i < myRooms.Size(); ++i)
		{
			if (myRooms[i]->Inside(aPosition) == true)
			{
				return i;
			}
		}

		DL_ASSERT("Unable to find room Id");
		return 0;
	}
}