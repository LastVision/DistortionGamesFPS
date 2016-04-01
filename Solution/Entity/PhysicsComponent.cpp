#include "stdafx.h"
#include <PhysicsCallbackStruct.h>
#include "PhysicsComponent.h"
#include "PhysicsComponentData.h"
#include <PhysicsInterface.h>
#include <CommonHelper.h>


PhysicsComponent::PhysicsComponent(Entity& aEntity, const PhysicsComponentData& aPhysicsComponentData
		, const std::string& aFBXPath)
	: Component(aEntity)
	, myData(aPhysicsComponentData)
	, myIsAwake(true)
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

	bool shouldAddToPhysicsScene = true;
	if (myEntity.GetType() == eEntityType::EXPLOSION || myEntity.GetSubType() == "respawn"
		|| myEntity.GetType() == eEntityType::BULLET || myEntity.GetSubType() == CU::ToLower("gunDroidServer")
		|| myEntity.GetSubType() == "gundroid")
	{
		shouldAddToPhysicsScene = false;
	}

	if (myPhysicsType != ePhysics::CAPSULE)
	{
		myCallbackStruct = Prism::PhysicsCallbackStruct(myData, std::bind(&PhysicsComponent::SwapOrientations, this), std::bind(&PhysicsComponent::UpdateOrientation, this));
		Prism::PhysicsInterface::GetInstance()->Create(this, myCallbackStruct, my4x4Float, aFBXPath, &myDynamicBody, &myStaticBody, &myShapes, shouldAddToPhysicsScene);
	}
	else if (myPhysicsType == ePhysics::CAPSULE)
	{
		myCapsuleControllerId = Prism::PhysicsInterface::GetInstance()->CreatePlayerController(myEntity.GetOrientation().GetPos(), this, shouldAddToPhysicsScene);
	}

	myIsInScene = shouldAddToPhysicsScene;
}


PhysicsComponent::~PhysicsComponent()
{
	delete[] myShapes;
	myShapes = nullptr;
}

void PhysicsComponent::Update(float)
{
	if (myPhysicsType == ePhysics::KINEMATIC && myIsAwake == true)
	{
		Prism::PhysicsInterface::GetInstance()->MoveToPosition(myDynamicBody, myEntity.GetOrientation().GetPos() + CU::Vector3<float>(0, 1.f, 0));
	}
	if (myIsAwake == false)
	{
		int apa;
		apa = 5;
	}
}

void PhysicsComponent::Reset()
{
	if (myDynamicBody != nullptr && myPhysicsType != ePhysics::KINEMATIC)
	{
		Prism::PhysicsInterface::GetInstance()->SetVelocity(myDynamicBody, CU::Vector3<float>(0, 0, 0));
	}
}

void PhysicsComponent::Sleep()
{
	myIsAwake = false;

	if (myDynamicBody != nullptr)
	{
		Prism::PhysicsInterface::GetInstance()->Sleep(myDynamicBody);
	}
	else if (myPhysicsType == ePhysics::CAPSULE)
	{
		Prism::PhysicsInterface::GetInstance()->Sleep(myCapsuleControllerId);
	}
	else
	{
		DL_ASSERT("Can't sleep non-dynamic body or non capsule.");
	}
}

void PhysicsComponent::Wake()
{
	myIsAwake = true;

	if (myDynamicBody != nullptr)
	{
		Prism::PhysicsInterface::GetInstance()->Wake(myDynamicBody);
	}
	else if (myPhysicsType == ePhysics::CAPSULE)
	{
		Prism::PhysicsInterface::GetInstance()->Wake(myCapsuleControllerId);
	}
	else
	{
		DL_ASSERT("Can't wake non-dynamic body or non capsule.");
	}
}

void PhysicsComponent::SwapOrientations()
{
	std::swap(my4x4Float, myThread4x4Float);
}

float* PhysicsComponent::GetPosition()
{
	//DL_ASSERT("Not used? Not verified to work.");
	return &myPosition[0];
}

float* PhysicsComponent::GetOrientation()
{
	return my4x4Float;
}

void PhysicsComponent::UpdateOrientation()
{
	DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant update Orientation on STATIC PhysEntities");

	if (myEntity.GetGID() == 60000)
	{
		int apa = 5;
		apa;
	}

	if (myIsAwake == true)
	{
		Prism::PhysicsInterface::GetInstance()->UpdateOrientation(myDynamicBody, myShapes, myThread4x4Float);

		myPosition[0] = myThread4x4Float[12];
		myPosition[1] = myThread4x4Float[13];
		myPosition[2] = myThread4x4Float[14];
	}
}

void PhysicsComponent::UpdateOrientationStatic()
{
	DL_ASSERT_EXP(myPhysicsType == ePhysics::PHANTOM, "Cant update Orientation Static on other types of PhysEntities");

	if (myIsAwake == true)
	{
		Prism::PhysicsInterface::GetInstance()->UpdateOrientation(myStaticBody, myShapes, myThread4x4Float);

		myPosition[0] = myThread4x4Float[12];
		myPosition[1] = myThread4x4Float[13];
		myPosition[2] = myThread4x4Float[14];
	}
}

void PhysicsComponent::AddForce(const CU::Vector3<float>& aDirection, float aMagnitude)
{
	DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");
	DL_ASSERT_EXP(myIsAwake == true, "Add force on sleeping object");

	Prism::PhysicsInterface::GetInstance()->AddForce(myDynamicBody, aDirection, aMagnitude);
}

void PhysicsComponent::SetVelocity(const CU::Vector3<float>& aVelocity)
{
	DL_ASSERT_EXP(myPhysicsType == ePhysics::DYNAMIC, "Cant add Force to STATIC objects");
	DL_ASSERT_EXP(myIsAwake == true, "Add force on sleeping object");

	Prism::PhysicsInterface::GetInstance()->SetVelocity(myDynamicBody, aVelocity);
}

void PhysicsComponent::TeleportToPosition(const CU::Vector3<float>& aPosition)
{
	DL_ASSERT_EXP(myPhysicsType != ePhysics::STATIC, "Cant add Force to STATIC objects");

	if (myDynamicBody != nullptr)
	{
		Prism::PhysicsInterface::GetInstance()->TeleportToPosition(myDynamicBody, aPosition);
	}
	else if (myStaticBody != nullptr)
	{
		Prism::PhysicsInterface::GetInstance()->TeleportToPosition(myStaticBody, aPosition);
	}
}

void PhysicsComponent::MoveToPosition(const CU::Vector3<float>& aPosition)
{
	DL_ASSERT_EXP(myPhysicsType != ePhysics::STATIC, "Cant add Force to STATIC objects");
	DL_ASSERT_EXP(myIsAwake == true, "Add force on sleeping object");

	Prism::PhysicsInterface::GetInstance()->MoveToPosition(myDynamicBody, aPosition);
}

void PhysicsComponent::SetPlayerCapsulePosition(const CU::Vector3<float>& aPosition)
{
	Prism::PhysicsInterface::GetInstance()->SetPosition(myCapsuleControllerId, aPosition);
}

void PhysicsComponent::AddToScene()
{
	if (myPhysicsType == ePhysics::DYNAMIC || myPhysicsType == ePhysics::KINEMATIC)
	{
		Prism::PhysicsInterface::GetInstance()->Add(myDynamicBody);
	}
	else if (myPhysicsType == ePhysics::CAPSULE)
	{
		Prism::PhysicsInterface::GetInstance()->Add(myCapsuleControllerId);
	}
	else
	{
		Prism::PhysicsInterface::GetInstance()->Add(myStaticBody);
	}
	myIsInScene = true;
}

void PhysicsComponent::RemoveFromScene()
{
	if (myPhysicsType == ePhysics::DYNAMIC || myPhysicsType == ePhysics::KINEMATIC)
	{
		Prism::PhysicsInterface::GetInstance()->Remove(myDynamicBody, myData);
	}
	else if (myPhysicsType == ePhysics::CAPSULE)
	{
		Prism::PhysicsInterface::GetInstance()->Remove(myCapsuleControllerId);
	}
	else
	{
		Prism::PhysicsInterface::GetInstance()->Remove(myStaticBody, myData);
	}
	myIsInScene = false;
}

