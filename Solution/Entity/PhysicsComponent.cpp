#include "stdafx.h"
#include <PhysicsCallbackStruct.h>
#include "PhysicsComponent.h"
#include "PhysicsComponentData.h"
#include <PhysicsInterface.h>


PhysicsComponent::PhysicsComponent(Entity& aEntity, const PhysicsComponentData& aPhysicsComponentData
	, const std::string& aFBXPath)
	: Component(aEntity)
	, myData(aPhysicsComponentData)
{
	for (int i = 0; i < 16; ++i)
	{
		my4x4Float[i] = myEntity.GetOrientation().myMatrix[i];
		myThread4x4Float[i] = myEntity.GetOrientation().myMatrix[i];
	}

	myPosition[0] = my4x4Float[12];
	myPosition[1] = my4x4Float[13];
	myPosition[2] = my4x4Float[14];

	myPhysicsType = aPhysicsComponentData.myPhysicsType;

	myCallbackStruct = Prism::PhysicsCallbackStruct(myData, std::bind(&PhysicsComponent::SwapOrientations, this), std::bind(&PhysicsComponent::UpdateOrientation, this));

	Prism::PhysicsInterface::GetInstance()->Create(this, myCallbackStruct, my4x4Float, aFBXPath, &myDynamicBody, &myStaticBody, &myShapes);
}


PhysicsComponent::~PhysicsComponent()
{
	delete[] myShapes;
	myShapes = nullptr;
}

void PhysicsComponent::Update(float aDelta)
{
	aDelta;
}

void PhysicsComponent::SwapOrientations()
{
	std::swap(my4x4Float, myThread4x4Float);
}

float* PhysicsComponent::GetPosition()
{
	DL_ASSERT("Not used? Not verified to work.");
	return &my4x4Float[12];
}

float* PhysicsComponent::GetOrientation()
{
	return my4x4Float;
}

void PhysicsComponent::UpdateOrientation()
{
	DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant update Orientation on STATIC PhysEntities");

	Prism::PhysicsInterface::GetInstance()->UpdateOrientation(myDynamicBody, myShapes, myThread4x4Float);


	myPosition[0] = myThread4x4Float[12];
	myPosition[1] = myThread4x4Float[13];
	myPosition[2] = myThread4x4Float[14];
}

void PhysicsComponent::AddForce(const CU::Vector3<float>& aDirection, float aMagnitude)
{
	DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");

	Prism::PhysicsInterface::GetInstance()->AddForce(myDynamicBody, aDirection, aMagnitude);
}

void PhysicsComponent::SetVelocity(const CU::Vector3<float>& aVelocity)
{
	DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");

	Prism::PhysicsInterface::GetInstance()->SetVelocity(myDynamicBody, aVelocity);
}

void PhysicsComponent::SetPosition(const CU::Vector3<float>& aPosition)
{
	DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");

	Prism::PhysicsInterface::GetInstance()->SetPosition(myDynamicBody, aPosition);
}

void PhysicsComponent::RemoveFromScene()
{
	if (myPhysicsType == ePhysics::DYNAMIC)
	{
		//PhysicsInterface::GetInstance()->Remove(myDynamicBody);
	}
	else
	{
		//PhysicsInterface::GetInstance()->Remove(myStaticBody);
	}

}

