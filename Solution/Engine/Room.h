#pragma once
#include <AABB.h>

namespace Prism
{
	class Portal;

	class Room
	{
	public:
		Room(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aScale, int aRoomId);
		~Room();

		bool Inside(const CU::Vector3<float>& aPosition) const;
		bool Collide(const Room& aRoom) const;

		const CU::Intersection::AABB& GetAABB() const;
		int GetRoomId() const;

		void AddPortal(Portal* aPortal);
		const CU::GrowingArray<Portal*>& GetPortals() const;

	private:
		void operator=(Room&) = delete;
		const int myRoomId;
		CU::Intersection::AABB myAABB;
		CU::GrowingArray<Portal*> myPortals;
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
}