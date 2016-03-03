#pragma once
#include <AABB.h>

namespace Prism
{
	class Portal;

	enum class eRoomType
	{
		ROOM,
		CONNECTOR,
	};

	class Room
	{
	public:
		Room(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aScale, int aRoomId, const std::string& aName, eRoomType aType);
		~Room();

		bool Inside(const CU::Vector3<float>& aPosition) const;
		bool Collide(const Room& aRoom) const;

		const CU::Intersection::AABB& GetAABB() const;
		int GetRoomId() const;

		void AddPortal(Portal* aPortal);
		const CU::GrowingArray<Portal*>& GetPortals() const;

		const std::string& GetName() const;
		eRoomType GetType() const;

	private:
		void operator=(Room&) = delete;
		const int myRoomId;
		CU::Intersection::AABB myAABB;
		CU::GrowingArray<Portal*> myPortals;
		const std::string myName;
		const eRoomType myType;
	};

	inline const CU::Intersection::AABB& Room::GetAABB() const
	{
		return myAABB;
	}

	inline int Room::GetRoomId() const
	{
		return myRoomId;
	}

	inline const CU::GrowingArray<Portal*>& Room::GetPortals() const
	{
		return myPortals;
	}

	inline const std::string& Room::GetName() const
	{
		return myName;
	}

	inline eRoomType Room::GetType() const
	{
		return myType;
	}
}