#include "stdafx.h"
#include <Intersection.h>
#include "Portal.h"
#include "Room.h"

namespace Prism
{
	Portal::Portal(const Room* aRoom0, const Room* aRoom1)
	{
		myRooms[0] = aRoom0;
		myRooms[1] = aRoom1;

		const CU::Intersection::AABB& aabb0 = myRooms[0]->GetAABB();
		const CU::Intersection::AABB& aabb1 = myRooms[1]->GetAABB();

		bool success = FindCollisionPlane(aabb0, aabb1);

		if (success == false)
		{
			success = FindCollisionPlane(aabb1, aabb0);
		}
		DL_ASSERT_EXP(success == true, "Failed to create portal.");
	}


	Portal::~Portal()
	{
	}

	bool Portal::FindCollisionPlane(const CU::Intersection::AABB& anAABB0, const CU::Intersection::AABB& anAABB1)
	{
		CU::Vector3<float> cube[8];
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
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[0], cube[5]) == true) // left side totally inside
		{
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[0], cube[7]) == true) // front side totally inside
		{
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[4], cube[6]) == true) // top side totally inside
		{
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[1], cube[6]) == true) // back side totally inside
		{
			return true;
		}
		else if (PlaneInsideAABB(anAABB1, cube[3], cube[6]) == true) // right side totally inside
		{
			return true;
		}

		return false;
	}

	bool Portal::PlaneInsideAABB(const CU::Intersection::AABB& anAABB, const CU::Vector3<float>& aMin, const CU::Vector3<float>& aMax) const
	{
		return CU::Intersection::PointInsideAABB(anAABB, aMin) && CU::Intersection::PointInsideAABB(anAABB, aMax);
	}
}