#include "stdafx.h"
#include <Intersection.h>
#include "Portal.h"
#include "Room.h"

namespace Prism
{
	Portal::Portal(Room* aRoom0, Room* aRoom1)
		: myAlreadyPassed(false)
	{
		mySmallRoom = aRoom1;
		myLargeRoom = aRoom0;

		const CU::Intersection::AABB& aabb0 = mySmallRoom->GetAABB();
		const CU::Intersection::AABB& aabb1 = myLargeRoom->GetAABB();

		bool success = FindCollisionPlane(aabb0, aabb1);

		if (success == false)
		{
			success = FindCollisionPlane(aabb1, aabb0);
			Room* temp = myLargeRoom;
			myLargeRoom = mySmallRoom;
			mySmallRoom = temp;
		}
		DL_ASSERT_EXP(success == true, "Failed to create portal.");

		myAABB.InitWithTwoPoints(myBottomLeft, myTopRight);

		myCenterPosition = (myBottomLeft + myTopRight) * 0.5f;

		myRadius = CU::Length(myTopRight - myCenterPosition);

		mySmallRoom->AddPortal(this);
		myLargeRoom->AddPortal(this);
	}


	Portal::~Portal()
	{
	}

	Room* Portal::GetOther(Room* aRoom)
	{
		if (aRoom == myLargeRoom)
		{
			return mySmallRoom;
		}
		DL_ASSERT_EXP(aRoom == mySmallRoom, "Room not associated with portal");
		return myLargeRoom;
	}

	bool Portal::FindCollisionPlane(const CU::Intersection::AABB& anAABB0, const CU::Intersection::AABB& anAABB1)
	{
		CU::Vector3<float> cube[8]; // clockwise cube as seen from above, lower vertices first
		cube[0] = anAABB0.myCenterPos + anAABB0.myExtents * CU::Vector3<float>(-0.5f, -0.5f, -0.5f);
		cube[1] = anAABB0.myCenterPos + anAABB0.myExtents * CU::Vector3<float>(-0.5f, -0.5f, +0.5f);
		cube[2] = anAABB0.myCenterPos + anAABB0.myExtents * CU::Vector3<float>(+0.5f, -0.5f, +0.5f);
		cube[3] = anAABB0.myCenterPos + anAABB0.myExtents * CU::Vector3<float>(+0.5f, -0.5f, -0.5f);
		cube[4] = anAABB0.myCenterPos + anAABB0.myExtents * CU::Vector3<float>(-0.5f, +0.5f, -0.5f);
		cube[5] = anAABB0.myCenterPos + anAABB0.myExtents * CU::Vector3<float>(-0.5f, +0.5f, +0.5f);
		cube[6] = anAABB0.myCenterPos + anAABB0.myExtents * CU::Vector3<float>(+0.5f, +0.5f, +0.5f);
		cube[7] = anAABB0.myCenterPos + anAABB0.myExtents * CU::Vector3<float>(+0.5f, +0.5f, -0.5f);

		if (PlaneInsideAABB(anAABB1, cube[0], cube[2]) == true) // bottom side totally inside
		{
			myBottomLeft.y = anAABB1.myMaxPos.y;
			myTopRight.y = anAABB1.myMaxPos.y;
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[0], cube[5]) == true) // left side totally inside
		{
			myBottomLeft.x = anAABB1.myMaxPos.x;
			myTopRight.x = anAABB1.myMaxPos.x;
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[0], cube[7]) == true) // front side totally inside
		{
			myBottomLeft.z = anAABB1.myMaxPos.z;
			myTopRight.z = anAABB1.myMaxPos.z;
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[4], cube[6]) == true) // top side totally inside
		{
			myBottomLeft.y = anAABB1.myMinPos.y;
			myTopRight.y = anAABB1.myMinPos.y;
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[3], cube[6]) == true) // right side totally inside
		{
			myBottomLeft.x = anAABB1.myMinPos.x;
			myTopRight.x = anAABB1.myMinPos.x;
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[1], cube[6]) == true) // back side totally inside
		{
			myBottomLeft.z = anAABB1.myMinPos.z;
			myTopRight.z = anAABB1.myMinPos.z;
			return true;
		}

		return false;
	}

	bool Portal::PlaneInsideAABB(const CU::Intersection::AABB& anAABB, const CU::Vector3<float>& aMin, const CU::Vector3<float>& aMax)
	{
		if (CU::Intersection::PointInsideAABB(anAABB, aMin) && CU::Intersection::PointInsideAABB(anAABB, aMax) == true)
		{
			myBottomLeft = aMin;
			myTopRight = aMax;
			return true;
		}

		return false;
	}
}