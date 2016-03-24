#include "stdafx.h"
#include <Intersection.h>
#include "Room.h"


namespace Prism
{
	Room::Room(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aScale
			, const std::string& aName, eRoomType aType)
		: myAABB(aPosition, aScale)
		, myPortals(8)
		, myInstances(128)
		, myName(aName)
		, myType(aType)
	{
	}

	Room::~Room()
	{
		myPortals.RemoveAll();
		myInstances.RemoveAll();
	}

	bool Room::Inside(const CU::Vector3<float>& aPosition, float aRadius) const
	{
		return CU::Intersection::SphereInsideAABB(myAABB, aPosition, aRadius);
	}

	bool Room::Collide(const Room& aRoom) const
	{
		return CU::Intersection::AABBvsAABB(myAABB, aRoom.myAABB);
	}

	void Room::Add(Portal* aPortal)
	{
		myPortals.Add(aPortal);
	}

	void Room::Add(Instance* anInstance)
	{
		myInstances.Add(anInstance);
	}
}