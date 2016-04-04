#pragma once
#include "Frustum.h"
#include <GrowingArray.h>

namespace Prism
{
	class Instance;
	class Portal;
	class Room;
	class PointLight;

	class RoomManager
	{
	public:
		RoomManager();
		~RoomManager();

		void Add(Room* aRoom);
		void CalcPortals();
		void Add(Instance* anInstance, eObjectRoomType aRoomType);
		void Add(PointLight* aPointLight);
		void Remove(Instance* anInstance);

		const CU::GrowingArray<Instance*>& GetActiveInstances(const Camera& aCamera);
		const CU::GrowingArray<Instance*>& GetAllInstances();
		const CU::GrowingArray<PointLight*>& GetActivePointLights();

	private:
		int GetRoomId(const CU::Vector3<float>& aPosition) const;
		void FindActiveRooms(Frustum aFrustum, const CU::Matrix44<float>& aCameraOrientation, int aRoomId, Portal* anArrivePortal = nullptr);
		bool AnyOutsidePlane(int somePlaneIndices[4], int aPlaneIndex) const;
		bool AllPointsBehind(bool somePoints[4]) const;

		struct InstanceInRoom
		{
			InstanceInRoom(){}
			InstanceInRoom(int aRoomId, Instance* anInstance)
				: myRoomId(aRoomId)
				, myInstance(anInstance)
			{}
			int myRoomId;
			Instance* myInstance;
		};

		CU::GrowingArray<Room*> myRooms;
		CU::GrowingArray<Portal*> myPortals;

		CU::GrowingArray<Instance*> myAlwaysRenderInstances;
		CU::GrowingArray<Instance*> myActiveInstances;
		CU::GrowingArray<Instance*> myAllInstances;
		CU::GrowingArray<PointLight*> myActivePointLights;

		bool myDebugDraw;

		int myTotalObjects;
		int myObjectsInDuplicateRooms;
	};
}