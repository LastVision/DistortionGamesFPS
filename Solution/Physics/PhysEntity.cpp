#include "stdafx.h"



#include "PhysEntity.h"
#include <PxPhysics.h>
#include <pxphysicsapi.h>

#include "PhysicsInterface.h"
#include "PhysicsManager.h"

namespace Prism
{
	PhysEntity::PhysEntity(const PhysEntityData& aPhysData
		, const CU::Matrix44<float>& aOrientation, const std::string& aFBXPath
		, Entity* aEntity)
		: myShapes(nullptr)
		, myEntity(aEntity)
	{
		for (int i = 0; i < 16; ++i)
		{
			my4x4Float[i] = aOrientation.myMatrix[i];
			myThread4x4Float[i] = aOrientation.myMatrix[i];
		}

		myPosition[0] = my4x4Float[12];
		myPosition[1] = my4x4Float[13];
		myPosition[2] = my4x4Float[14];

		myPhysicsType = aPhysData.myPhysics;

		PhysicsInterface::GetInstance()->Create(this, aPhysData, my4x4Float, aFBXPath, &myDynamicBody, &myStaticBody, &myShapes);
	}


	PhysEntity::~PhysEntity()
	{
		delete[] myShapes;
		myShapes = nullptr;
	}

	void PhysEntity::SwapOrientations()
	{
		std::swap(my4x4Float, myThread4x4Float);
	}

	float* PhysEntity::GetPosition()
	{
		DL_ASSERT("Not used? Not verified to work.");
		return &my4x4Float[12];
	}

	float* PhysEntity::GetOrientation()
	{
		return my4x4Float;
	}

	void PhysEntity::UpdateOrientation()
	{
		DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant update Orientation on STATIC PhysEntities");

		physx::PxU32 nShapes = myDynamicBody->getNbShapes();
		myDynamicBody->getShapes(myShapes, nShapes);

		physx::PxTransform pT = physx::PxShapeExt::getGlobalPose(*myShapes[0], *myDynamicBody);
		physx::PxTransform graphicsTransform(pT.p, pT.q);
		physx::PxMat33 m = physx::PxMat33(graphicsTransform.q);

		myThread4x4Float[0] = m(0, 0);
		myThread4x4Float[1] = m(1, 0);
		myThread4x4Float[2] = m(2, 0);
		myThread4x4Float[3] = 0;

		myThread4x4Float[4] = m(0, 1);
		myThread4x4Float[5] = m(1, 1);
		myThread4x4Float[6] = m(2, 1);
		myThread4x4Float[7] = 0;

		myThread4x4Float[8] = m(0, 2);
		myThread4x4Float[9] = m(1, 2);
		myThread4x4Float[10] = m(2, 2);
		myThread4x4Float[11] = 0;

		myThread4x4Float[12] = graphicsTransform.p.x;
		myThread4x4Float[13] = graphicsTransform.p.y;
		myThread4x4Float[14] = graphicsTransform.p.z;
		myThread4x4Float[15] = 1;


		myPosition[0] = graphicsTransform.p.x;
		myPosition[1] = graphicsTransform.p.y;
		myPosition[2] = graphicsTransform.p.z;
	}

	void PhysEntity::AddForce(const CU::Vector3<float>& aDirection, float aMagnitude)
	{
		DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");

		PhysicsInterface::GetInstance()->AddForce(myDynamicBody, aDirection, aMagnitude);
	}

	void PhysEntity::SetVelocity(const CU::Vector3<float>& aVelocity)
	{
		DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");

		PhysicsInterface::GetInstance()->SetVelocity(myDynamicBody, aVelocity);
	}

	void PhysEntity::SetPosition(const CU::Vector3<float>& aPosition)
	{
		DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");

		PhysicsInterface::GetInstance()->SetPosition(myDynamicBody, aPosition);
	}

	void PhysEntity::RemoveFromScene()
	{
		if (myPhysicsType == ePhysics::DYNAMIC)
		{
			PhysicsInterface::GetInstance()->Remove(myDynamicBody);
		}
		else
		{
			PhysicsInterface::GetInstance()->Remove(myStaticBody);
		}
		
	}
}