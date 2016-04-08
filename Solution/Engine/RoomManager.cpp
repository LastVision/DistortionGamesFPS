#include "stdafx.h"
#include "Camera.h"
#include <CommonHelper.h>
#include "Frustum.h"
#include <InputWrapper.h>
#include "Instance.h"
#include <Intersection.h>
#include "ModelProxy.h"
#include "Portal.h"
#include "Room.h"
#include "RoomManager.h"
#include "PointLight.h"
#include "SpotLight.h"

namespace Prism
{
	RoomManager::RoomManager()
		: myRooms(128)
		, myPortals(128)
		, myAlwaysRenderInstances(128)
		, myActivePointLights(512)
		, myActiveSpotLights(512)
		, myActiveInstances(4096)
		, myAllInstances(4096)
		, myDebugDraw(false)
		, myTotalObjects(0)
		, myObjectsInDuplicateRooms(0)
	{
	}


	RoomManager::~RoomManager()
	{
		myActiveInstances.RemoveAll();
		myPortals.DeleteAll();
		myRooms.DeleteAll();
	}

	void RoomManager::Add(Room* aRoom)
	{
		myRooms.Add(aRoom);
	}

	static bool cmp(Room* a, Room* b)
	{
		return int(a->GetType()) < int(b->GetType());
	}


	void RoomManager::CalcPortals()
	{
		std::sort(myRooms.begin(), myRooms.end(), cmp);

		for (int i = 0; i < myRooms.Size(); ++i)
		{
			myRooms[i]->SetRoomId(i);
		}

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

		//bool success = false;
		//while (success == false)
		//{
		//	success = true;
		//	for (int i = 0; i < myRooms.Size() - 1; ++i)
		//	{
		//		if (myRooms[i + 1] < myRooms[i])
		//		{
		//			std::swap(myRooms[i], myRooms[i + 1]);
		//			success = false;
		//		}
		//	}
		//}
	}

	void RoomManager::Add(Instance* anInstance, eObjectRoomType aRoomType)
	{
		++myTotalObjects;
		myAllInstances.Add(anInstance);
		DL_ASSERT_EXP(aRoomType != eObjectRoomType::NONE, "Can't add object room type NONE to Room Manager.");
		if (aRoomType != eObjectRoomType::STATIC)
		{
			myAlwaysRenderInstances.Add(anInstance);
		}
		else
		{
			bool success(false);

			for (int i = 0; i < myRooms.Size(); ++i)
			{
				if (myRooms[i]->Inside(anInstance->GetPosition(), anInstance->GetModel().GetRadius()) == true)
				{
					if (success == true)
					{
						anInstance->SetExistInMultipleRooms(true);
						++myObjectsInDuplicateRooms;
					}
					//myInstances.Add(InstanceInRoom(i, anInstance));
					myRooms[i]->Add(anInstance);
					success = true;
					//break;
				}
			}

			DL_ASSERT_EXP(success == true, CU::Concatenate("Instance found outside room: (x: %f y: %f z: %f)"
				, anInstance->GetOrientation().GetPos().x
				, anInstance->GetOrientation().GetPos().y
				, anInstance->GetOrientation().GetPos().z));
		}
	}

	void RoomManager::Add(PointLight* aPointLight)
	{
		for (int i = 0; i < myRooms.Size(); ++i)
		{
			if (myRooms[i]->Inside(aPointLight->GetPosition().GetVector3(), aPointLight->GetRange()) == true)
			{
				myRooms[i]->Add(aPointLight);
			}
		}
	}

	void RoomManager::Add(SpotLight* aSpotLight)
	{
		for (int i = 0; i < myRooms.Size(); ++i)
		{
			if (myRooms[i]->Inside(aSpotLight->GetPosition().GetVector3(), aSpotLight->GetRange()) == true)
			{
				myRooms[i]->Add(aSpotLight);
			}
		}
	}

	void RoomManager::Remove(Instance* anInstance)
	{
		for (int i = 0; i < myAlwaysRenderInstances.Size(); ++i)
		{
			if (myAlwaysRenderInstances[i] == anInstance)
			{
				--myTotalObjects;
				myAlwaysRenderInstances.RemoveCyclicAtIndex(i);
				return;
			}
		}

		bool addAssertHereWhenGrenadeRemoveIsWorking = true;
		//DL_ASSERT("Unable to remove instance that does either not exist or does not have the \"AlwaysRender\" bool set to true.");
	}

	const CU::GrowingArray<Instance*>& RoomManager::GetActiveInstances(const Camera& aCamera)
	{
#ifndef RELEASE_BUILD
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_P) == true)
		{
			myDebugDraw = !myDebugDraw;
		}
#endif
		myActivePointLights.RemoveAll();
		myActiveSpotLights.RemoveAll();
		myActiveInstances.RemoveAll();

		for (int i = 0; i < myPortals.Size(); ++i)
		{
			myPortals[i]->SetAlreadyPassed(false);
		}

		myPlayerRoom = GetRoomId(aCamera.GetOrientation().GetPos());

		FindActiveRooms(aCamera.GetFrustum(), aCamera.GetOrientation(), myPlayerRoom);

		//for (int i = 0; i < myCurrentRoomIds.Size(); ++i)
		//{
		//	const Room* current(myRooms[myCurrentRoomIds[i]]);
		//	if (current->GetType() == eRoomType::ROOM)
		//	{
		//		for (int j = 0; j < current->GetPortals().Size(); ++j)
		//		{
		//			const Room* other(current->GetPortals()[j]->GetOther(current));
		//			if (myCurrentRoomIds.Find(other->GetRoomId()) == myCurrentRoomIds.FoundNone)
		//			{
		//				myCurrentRoomIds.Add(other->GetRoomId());
		//			}
		//		}
		//	}
		//}

		for each (Instance* instance in myAlwaysRenderInstances)
		{
			myActiveInstances.Add(instance);
		}

		//for each (const InstanceInRoom& instance in myInstances)
		//{
		//	for each (int id in myCurrentRoomIds)
		//	{
		//		if (instance.myRoomId == id)
		//		{
		//			myActiveInstances.Add(instance.myInstance);
		//			break;
		//		}
		//	}
		//}



#ifndef RELEASE_BUILD
#ifdef SHOW_PORTAL_CULLING_DEBUG_TEXT
		DEBUG_PRINT(myRooms[myPlayerRoom]->GetName());
		DEBUG_PRINT(myActiveInstances.Size());
		DEBUG_PRINT(myTotalObjects);
		DEBUG_PRINT(myObjectsInDuplicateRooms);
		float objectsInDuplicateRoomsPercent = 100.f * static_cast<float>(myObjectsInDuplicateRooms) / static_cast<float>(myTotalObjects);
		DEBUG_PRINT(objectsInDuplicateRoomsPercent);
		DEBUG_PRINT(myActivePointLights.Size());
		DEBUG_PRINT(myActiveSpotLights.Size());
		//DEBUG_PRINT(myInstances.Size());
		//float renderPercentage = 100.f * float(myActiveInstances.Size())
		//	/ (myInstances.Size() + myAlwaysRenderInstances.Size());
		//DEBUG_PRINT(renderPercentage);
		//for (int j = 0; j < myCurrentRoomIds.Size(); ++j)
		//{
		//	const std::string& roomName(myRooms[myCurrentRoomIds[j]]->GetName());
		//	if (j == 0)
		//	{
		//		const std::string& playerRoom(roomName);
		//		DEBUG_PRINT(playerRoom);
		//	}
		//	DEBUG_PRINT(roomName);
		//}
		//if (renderPercentage > 25.f)
		//{
		//	for (float i = 25.f; i < renderPercentage; i += 5.f)
		//	{
		//		DEBUG_PRINT("WARNING, rendering huge part of level");

		//	}
		//}
#endif
#endif
		return myActiveInstances;
	}

	const CU::GrowingArray<Instance*>& RoomManager::GetAllInstances()
	{
		return myAllInstances;
	}

	const CU::GrowingArray<PointLight*>& RoomManager::GetActivePointLights()
	{
		return myActivePointLights;
	}

	const CU::GrowingArray<SpotLight*>& RoomManager::GetActiveSpotLights()
	{
		return myActiveSpotLights;
	}

	Prism::Room* RoomManager::GetPlayerRoom()
	{
		if (myCurrentPlayerRoom != myRooms[myPlayerRoom])
		{
			myPreviousPlayerRoom = myCurrentPlayerRoom;
			myCurrentPlayerRoom = myRooms[myPlayerRoom];
		}
		return myCurrentPlayerRoom;
	}

	Prism::Room* RoomManager::GetPreviousPlayerRoom()
	{
		return myPreviousPlayerRoom;
	}

	int RoomManager::GetRoomId(const CU::Vector3<float>& aPosition) const
	{
		for (int i = 0; i < myRooms.Size(); ++i)
		{
			if (myRooms[i]->Inside(aPosition, 0) == true)
			{
				return i;
			}
		}

		DL_ASSERT("Unable to find room Id");
		return 0;
	}

	void RoomManager::FindActiveRooms(Frustum aFrustum, const CU::Matrix44<float>& aCameraOrientation, int aRoomId, Portal* anArrivePortal)
	{
		if (anArrivePortal != nullptr)
		{
			aFrustum.Resize(anArrivePortal, aCameraOrientation, myDebugDraw);
		}
		aFrustum.CalcWorldPlanes();

		for each (Instance* instance in myRooms[aRoomId]->GetInstances())
		{
			if (aFrustum.Inside(instance->GetPosition(), instance->GetModel().GetRadius()) == true)
			{
				if (instance->GetExistInMultipleRooms() == false
					|| myActiveInstances.Find(instance) == myActiveInstances.FoundNone)
				{
					myActiveInstances.Add(instance);

				}
			}
		}

		for each (PointLight* light in myRooms[aRoomId]->GetPointLights())
		{
			if (aFrustum.Inside(light->GetPosition().GetVector3(), light->GetRange()) == true)
			{
				if (myActivePointLights.Find(light) == myActivePointLights.FoundNone)
				{
					myActivePointLights.Add(light);
				}
			}
		}

		for each (SpotLight* light in myRooms[aRoomId]->GetSpotLights())
		{
			if (aFrustum.Inside(light->GetPosition().GetVector3(), light->GetRange()) == true)
			{
				if (myActiveSpotLights.Find(light) == myActiveSpotLights.FoundNone)
				{
					myActiveSpotLights.Add(light);
				}
			}
		}

		if (aFrustum.GetBottomLeft() != CU::Vector3<float>(0, 0, 0))
		{
			//Prism::DebugDrawer::GetInstance()->RenderLine3D(aFrustum.GetOrientation().GetPos(), aFrustum.GetBottomLeft(), eColorDebug::RED);
			//Prism::DebugDrawer::GetInstance()->RenderLine3D(aFrustum.GetOrientation().GetPos(), aFrustum.GetTopRight(), eColorDebug::BLUE);
		}

		const CU::GrowingArray<Portal*>& portals = myRooms[aRoomId]->GetPortals();

		for (int i = 0; i < portals.Size(); ++i)
		{
			Portal* current = portals[i];
			int planeOutside[4];
			bool pointBehind[4];




			//if (current->GetAlreadyPassed() == false
			//	&& (aFrustum.Inside(current->GetPoint(0), 0, planeOutside[0], pointBehind[0]) == true
			//	|| aFrustum.Inside(current->GetPoint(1), 0, planeOutside[1], pointBehind[1]) == true
			//	|| aFrustum.Inside(current->GetPoint(2), 0, planeOutside[2], pointBehind[2]) == true
			//	|| aFrustum.Inside(current->GetPoint(3), 0, planeOutside[3], pointBehind[3]) == true

			//	//|| AllPointsBehind(pointBehind) == false // fancy solution, still not 100% working
			//	//&& (AnyOutsidePlane(planeOutside, 0) == true && AnyOutsidePlane(planeOutside, 1)
			//	//|| AnyOutsidePlane(planeOutside, 2) == true && AnyOutsidePlane(planeOutside, 3))))
			//	
			//	|| AllPointsBehind(pointBehind) == false && !(planeOutside[0] == planeOutside[1] && planeOutside[0] == planeOutside[2] && planeOutside[0] == planeOutside[3]))) //naive solution
			//	
			//	//|| aFrustum.Inside(current->GetCenterPosition(), current->GetRadius() * 0.1f) == true)) // needs to be tweaked for portal size


			if (current->GetAlreadyPassed() == false
				&& (aFrustum.CheckAABBInside(current->GetBottomLeft(), current->GetTopRight()) == true))
			{
				if (myDebugDraw == true)
				{
					Prism::DebugDrawer::GetInstance()->RenderLine3D(current->GetPoint(0), current->GetPoint(1), eColorDebug::RED, eColorDebug::BLUE);
					Prism::DebugDrawer::GetInstance()->RenderLine3D(current->GetPoint(1), current->GetPoint(3), eColorDebug::BLUE, eColorDebug::GREEN);
					Prism::DebugDrawer::GetInstance()->RenderLine3D(current->GetPoint(3), current->GetPoint(2), eColorDebug::GREEN, eColorDebug::YELLOW);
					Prism::DebugDrawer::GetInstance()->RenderLine3D(current->GetPoint(2), current->GetPoint(0), eColorDebug::YELLOW, eColorDebug::RED);
				}
				current->SetAlreadyPassed(true);

				int otherRoomId = current->GetOther(myRooms[aRoomId])->GetRoomId();

			//	FindActiveRooms(aFrustum, aCameraOrientation, otherRoomId, current);
			}
		}
	}

	bool RoomManager::AnyOutsidePlane(int somePlaneIndices[4], int aPlaneIndex) const
	{
		return somePlaneIndices[0] == aPlaneIndex
			|| somePlaneIndices[1] == aPlaneIndex
			|| somePlaneIndices[2] == aPlaneIndex
			|| somePlaneIndices[3] == aPlaneIndex;
	}

	bool RoomManager::AllPointsBehind(bool somePoints[4]) const
	{
		return somePoints[0] == true
			&& somePoints[1] == true
			&& somePoints[2] == true
			&& somePoints[3] == true;
	}
}