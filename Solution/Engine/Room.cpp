#include "stdafx.h"
#include <Intersection.h>
#include "Room.h"


namespace Prism
{
	Room::Room(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aScale)
		: myAABB(aPosition, aScale)
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
}