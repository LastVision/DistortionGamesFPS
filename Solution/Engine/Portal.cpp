#include "stdafx.h"
#include <CommonHelper.h>
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
		DL_ASSERT_EXP(success == true, CU::Concatenate("Failed to create portal between: (x: %f y: %f z: %f), (x: %f y: %f z: %f)"
			, aabb0.myCenterPos.x
			, aabb0.myCenterPos.y
			, aabb0.myCenterPos.z
			, aabb1.myCenterPos.x
			, aabb1.myCenterPos.y
			, aabb1.myCenterPos.z));

		myAABB.InitWithTwoPoints(myBottomLeftFront, myTopRightBack);

		myCenterPosition = (myBottomLeftFront + myTopRightBack) * 0.5f;

		myRadius = CU::Length(myTopRightBack - myCenterPosition);

		mySmallRoom->AddPortal(this);
		myLargeRoom->AddPortal(this);
		CalcPoints(); // tried to calc all 4 different corners of portal, not verified //Linus
	}


	Portal::~Portal()
	{
	}

	Room* Portal::GetOther(const Room* aRoom)
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
			myBottomLeftFront.y = anAABB1.myMaxPos.y;
			myTopRightBack.y = anAABB1.myMaxPos.y;
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[0], cube[5]) == true) // left side totally inside
		{
			myBottomLeftFront.x = anAABB1.myMaxPos.x;
			myTopRightBack.x = anAABB1.myMaxPos.x;
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[0], cube[7]) == true) // front side totally inside
		{
			myBottomLeftFront.z = anAABB1.myMaxPos.z;
			myTopRightBack.z = anAABB1.myMaxPos.z;
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[4], cube[6]) == true) // top side totally inside
		{
			myBottomLeftFront.y = anAABB1.myMinPos.y;
			myTopRightBack.y = anAABB1.myMinPos.y;
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[3], cube[6]) == true) // right side totally inside
		{
			myBottomLeftFront.x = anAABB1.myMinPos.x;
			myTopRightBack.x = anAABB1.myMinPos.x;
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[1], cube[6]) == true) // back side totally inside
		{
			myBottomLeftFront.z = anAABB1.myMinPos.z;
			myTopRightBack.z = anAABB1.myMinPos.z;
			return true;
		}

		return false;
	}

	bool Portal::PlaneInsideAABB(const CU::Intersection::AABB& anAABB, const CU::Vector3<float>& aMin, const CU::Vector3<float>& aMax)
	{
		if (CU::Intersection::PointInsideAABB(anAABB, aMin) && CU::Intersection::PointInsideAABB(anAABB, aMax) == true)
		{
			myBottomLeftFront = aMin;
			myTopRightBack = aMax;
			return true;
		}

		return false;
	}

	void Portal::CalcPoints()
	{
		float* lowX = nullptr;
		float* lowY = nullptr;
		float* highX = nullptr;
		float* highY = nullptr;
		float* z = nullptr;

		int dir = -1;
		if (myBottomLeftFront.x == myTopRightBack.x)
		{
			dir = 0;
			z = &myBottomLeftFront.x;
			lowX = &myBottomLeftFront.y;
			lowY = &myBottomLeftFront.z;
			highX = &myTopRightBack.y;
			highY = &myTopRightBack.z;
		}
		else if (myBottomLeftFront.y == myTopRightBack.y)
		{
			dir = 1;
			z = &myBottomLeftFront.y;
			lowX = &myBottomLeftFront.x;
			lowY = &myBottomLeftFront.z;
			highX = &myTopRightBack.x;
			highY = &myTopRightBack.z;
		}
		else
		{
			dir = 2;
			DL_ASSERT_EXP(myBottomLeftFront.z == myTopRightBack.z, "Portal not exact plane.");
			z = &myBottomLeftFront.z;
			lowX = &myBottomLeftFront.x;
			lowY = &myBottomLeftFront.y;
			highX = &myTopRightBack.x;
			highY = &myTopRightBack.y;
		}

		if (dir == 0)
		{
			myPoints[0] = myBottomLeftFront;
			
			myPoints[1] = myBottomLeftFront;
			myPoints[1].z = myTopRightBack.z;

			myPoints[2] = myBottomLeftFront;
			myPoints[2].y = myTopRightBack.y;

			myPoints[3] = myTopRightBack;
		}
		else if (dir == 1)
		{
			myPoints[0] = myBottomLeftFront;

			myPoints[1] = myBottomLeftFront;
			myPoints[1].x = myTopRightBack.x;

			myPoints[2] = myBottomLeftFront;
			myPoints[2].z = myTopRightBack.z;

			myPoints[3] = myTopRightBack;
		}
		else
		{
			DL_ASSERT_EXP(dir == 2, "unknown direction");
			myPoints[0] = myBottomLeftFront;

			myPoints[1] = myBottomLeftFront;
			myPoints[1].x = myTopRightBack.x;

			myPoints[2] = myBottomLeftFront;
			myPoints[2].y = myTopRightBack.y;

			myPoints[3] = myTopRightBack;
		}

		for (int i = 0; i < 4 - 1; ++i)
		{
			DL_ASSERT_EXP(myPoints[i] != myPoints[i + 1], "Portal points have same position");
		}
	}
}