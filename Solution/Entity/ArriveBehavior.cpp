#include "stdafx.h"
#include "ArriveBehavior.h"
#include "AIComponentData.h"

ArriveBehavior::ArriveBehavior(const Entity& anEntity, const AIComponentData& aData)
	: Behavior(anEntity, aData)
	, myTimeToTarget(0.1f)
	, myActive(false)
{
}


ArriveBehavior::~ArriveBehavior()
{
}

const CU::Vector3<float>& ArriveBehavior::Update(float)
{
	if (myActive == false) // can remove?
	{
		myVelocity.x = 0;
		myVelocity.z = 0;
		return myVelocity;
	}


	CU::Vector3<float> direction = myTarget - myEntity.GetOrientation().GetPos();
	float distance2 = CU::Length2(direction);

	float targetSpeed = myData.mySpeed;
	if (distance2 > myData.myArriveStopRange * myData.myArriveStopRange)
	{
		myDone = false;
	}
	else
	{
		targetSpeed = 0.00001f;
		myDone = true;
	}

	//if (distance2 > mySlowRadius)
	//{
	//	targetSpeed = myMaxSpeed;
	//}
	//else
	//{
	//	targetSpeed = myMaxSpeed * distance / mySlowRadius;
	//}

	myVelocity = direction;
	CU::Normalize(myVelocity);
	myVelocity *= targetSpeed;

	return myVelocity;
}