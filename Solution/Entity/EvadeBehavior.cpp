#include "stdafx.h"

#include "EvadeBehavior.h"
#include "PollingStation.h"
#include <InputWrapper.h>

EvadeBehavior::EvadeBehavior(const Entity& anEntity)
	: Behavior(anEntity)
	, myMaxAcceleration(10000.f)
	, myEntitiesToEvade(128)
	, myPropSpeed(240.f)
	, myPropMod(50.f)
{
}

EvadeBehavior::~EvadeBehavior()
{
}

const CU::Vector3<float>& EvadeBehavior::Update(float aDelta)
{
	aDelta;
	return myVelocity;
	//myAcceleration.x = 0;
	//myAcceleration.y = 0;

	//myEntitiesToEvade.RemoveAll();
	//float radius = myEntity.GetComponent<CollisionComponent>()->GetRadius();
	//PollingStation::GetInstance()->FindAllUnitsCloseToEntity(&myEntity, radius, myEntitiesToEvade);
	//EvadeEntities(aDelta, 240.f);

	//myEntitiesToEvade.RemoveAll();
	//PollingStation::GetInstance()->FindAllPropsCloseToEntity(&myEntity, radius, myEntitiesToEvade);
	//EvadeEntities(aDelta, myPropSpeed);


	///*DEBUG_PRINT(myPropSpeed);
	//DEBUG_PRINT(myPropMod);

	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_O))
	//{
	//	myPropSpeed += myPropMod;
	//}
	//else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_L))
	//{
	//	myPropSpeed -= myPropMod;
	//}
	//else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_I))
	//{
	//	myPropMod += 10.f;
	//}
	//else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_K))
	//{
	//	myPropMod -= 10.f;
	//}*/


	//if (CU::Length(myAcceleration) > myMaxAcceleration)
	//{
	//	CU::Normalize(myAcceleration);
	//	myAcceleration *= myMaxAcceleration;
	//}

	//return myAcceleration;
}

void EvadeBehavior::EvadeEntities(float aDelta, float aEvadeSpeed)
{
	aEvadeSpeed;
	aDelta;
	//float radius = myEntity.GetComponent<CollisionComponent>()->GetRadius();

	//for (int i = 0; i < myEntitiesToEvade.Size(); ++i)
	//{
	//	CU::Vector3<float> toTarget = myEntity.GetPosition() - myEntitiesToEvade[i]->GetPosition();
	//	CollisionComponent* collision = myEntitiesToEvade[i]->GetComponent<CollisionComponent>();
	//	float targetRadius = 0.f;
	//	if (collision != nullptr)
	//	{
	//		targetRadius = collision->GetRadius();
	//	}

	//	float totalRadius = (radius + targetRadius);// *(radius + targetRadius);
	//	float distToTarget = CU::Length(toTarget);
	//	float diff = 1.f - (distToTarget / totalRadius);

	//	float speed = aEvadeSpeed * diff;
	//	myAcceleration += toTarget * speed;
	//}
}