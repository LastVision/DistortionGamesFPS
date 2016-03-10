#include "stdafx.h"



#include "PhysEntity.h"
//#include <PxPhysics.h>
//#include <pxphysicsapi.h>

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

		PhysicsInterface::GetInstance()->UpdateOrientation(myDynamicBody, myShapes, myThread4x4Float);




		myPosition[0] = myThread4x4Float[12];
		myPosition[1] = myThread4x4Float[13];
		myPosition[2] = myThread4x4Float[14];
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