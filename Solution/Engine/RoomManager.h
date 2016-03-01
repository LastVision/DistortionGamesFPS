#pragma once
#include <GrowingArray.h>


namespace Prism
{
	class Instance;
	class Room;

	class RoomManager
	{
	public:
		RoomManager();
		~RoomManager();

		void Add(Room* aRoom);
		void Add(Instance* anInstance);
		void Remove(Instance* anInstance);

		const CU::GrowingArray<Instance*>& GetActiveInstances(const Camera& aCamera);

	private:
		int GetRoomId(const CU::Vector3<float>& aPosition) const;
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

		CU::GrowingArray<InstanceInRoom> myInstances;
		CU::GrowingArray<Instance*> myActiveInstances;
	};
}