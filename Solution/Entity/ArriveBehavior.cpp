#include "stdafx.h"
#include "ArriveBehavior.h"


ArriveBehavior::ArriveBehavior(const Entity& anEntity)
	: Behavior(anEntity)
	, myTimeToTarget(0.1f)
	, mySlowRadius(4.0f)
	, myTargetRadius(3.f)
	, myMaxAcceleration(100000.f)
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
	float distance = CU::Length(direction);

	if (distance > myTargetRadius)
	{
		myDone = false;
	}
	else
	{
		myDone = true;
	}

	float targetSpeed;
	if (distance > mySlowRadius)
	{
		targetSpeed = myMaxSpeed;
	}
	else
	{
		targetSpeed = myMaxSpeed * distance / mySlowRadius;
	}

	myVelocity = direction;
	CU::Normalize(myVelocity);
	myVelocity *= targetSpeed;

	return myVelocity;

	//myAcceleration = targetVelocity - myEntity.GetVelocity();
	//myAcceleration /= myTimeToTarget;

	//if (CU::Length2(myAcceleration) > myMaxAcceleration * myMaxAcceleration)
	//{
	//	CU::Normalize(myAcceleration);
	//	myAcceleration *= myMaxAcceleration;
	//}

	//return myAcceleration;
}