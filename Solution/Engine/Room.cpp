#include "stdafx.h"
#include <Intersection.h>
#include "Room.h"


namespace Prism
{
	Room::Room(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aScale, int aRoomId
			, const std::string& aName, eRoomType aType)
		: myAABB(aPosition, aScale)
		, myPortals(8)
		, myRoomId(aRoomId)
		, myName(aName)
		, myType(aType)
	{
	}

	Room::~Room()
	{
	}

	bool Room::Inside(const CU::Vector3<float>& aPosition) const
	{
		return CU::Intersection::PointInsideAABB(myAABB, aPosition);
	}

	bool Room::Collide(const Room& aRoom) const
	{
		return CU::Intersection::AABBvsAABB(myAABB, aRoom.myAABB);
	}

	void Room::AddPortal(Portal* aPortal)
	{
		myPortals.Add(aPortal);
	}
}