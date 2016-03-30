#pragma once
#include <AABB.h>

namespace Prism
{
	class Portal;
	class PointLight;

	enum class eRoomType
	{
		ROOM,
		CONNECTOR,
	};

	class Room
	{
	public:
		Room(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aScale, const std::string& aName, eRoomType aType);
		~Room();

		bool Inside(const CU::Vector3<float>& aPosition, float aRadius) const;
		bool Collide(const Room& aRoom) const;

		const CU::Intersection::AABB& GetAABB() const;
		void SetRoomId(int aId);
		int GetRoomId() const;

		void Add(Portal* aPortal);
		const CU::GrowingArray<Portal*>& GetPortals() const;

		void Add(Instance* anInstance);
		const CU::GrowingArray<Instance*>& GetInstances() const;

		void Add(PointLight* aPointLight);
		const CU::GrowingArray<PointLight*>& GetPointLights() const;

		const std::string& GetName() const;
		eRoomType GetType() const;

		bool operator<(Room& aRoom);

	private:
		void operator=(Room&) = delete;
		int myRoomId;
		CU::Intersection::AABB myAABB;
		CU::GrowingArray<Portal*> myPortals;
		CU::GrowingArray<Instance*> myInstances;
		CU::GrowingArray<PointLight*> myPointLights;
		const std::string myName;
		const eRoomType myType;
	};

	inline const CU::Intersection::AABB& Room::GetAABB() const
	{
		return myAABB;
	}

	inline void Room::SetRoomId(int aId)
	{
		myRoomId = aId;
	}

	inline int Room::GetRoomId() const
	{
		return myRoomId;
	}

	inline const CU::GrowingArray<Portal*>& Room::GetPortals() const
	{
		return myPortals;
	}

	inline const CU::GrowingArray<Instance*>& Room::GetInstances() const
	{
		return myInstances;
	}

	inline const CU::GrowingArray<PointLight*>& Room::GetPointLights() const
	{
		return myPointLights;
	}

	inline const std::string& Room::GetName() const
	{
		return myName;
	}

	inline eRoomType Room::GetType() const
	{
		return myType;
	}

	inline bool Room::operator<(Room& aRoom)
	{
		return int(myType) < int(aRoom.myType);
	}
}