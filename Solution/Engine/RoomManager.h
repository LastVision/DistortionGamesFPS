#pragma once
#include "Frustum.h"
#include <GrowingArray.h>

namespace Prism
{
	class Instance;
	class Portal;
	class Room;

	class RoomManager
	{
	public:
		RoomManager();
		~RoomManager();

		void Add(Room* aRoom);
		void CalcPortals();
		void Add(Instance* anInstance, bool anAlwaysRender);
		void Remove(Instance* anInstance);

		const CU::GrowingArray<Instance*>& GetActiveInstances(const Camera& aCamera);

	private:
		int GetRoomId(const CU::Vector3<float>& aPosition) const;
		void FindActiveRooms(Frustum aFrustum, int aRoomId, Portal* anArrivePortal = nullptr);

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
		CU::GrowingArray<int> myCurrentRoomIds;

		CU::GrowingArray<InstanceInRoom> myInstances;
		CU::GrowingArray<Instance*> myAlwaysRenderInstances;
		CU::GrowingArray<Instance*> myActiveInstances;
	};
}