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

const CU::Vector2<float>& ArriveBehavior::Update(float)
{
	if (myActive == false) // can remove?
	{
		myAcceleration.x = 0;
		myAcceleration.y = 0;
		return myAcceleration;
	}


	CU::Vector2<float> direction = myTarget - myEntity.GetPosition();
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
		targetSpeed = myEntity.GetMaxSpeed();
	}
	else
	{
		targetSpeed = myEntity.GetMaxSpeed() * distance / mySlowRadius;
	}

	CU::Vector2<float> targetVelocity = direction;
	CU::Normalize(targetVelocity);
	targetVelocity *= targetSpeed;

	myAcceleration = targetVelocity - myEntity.GetVelocity();
	myAcceleration /= myTimeToTarget;

	if (CU::Length2(myAcceleration) > myMaxAcceleration * myMaxAcceleration)
	{
		CU::Normalize(myAcceleration);
		myAcceleration *= myMaxAcceleration;
	}

	return myAcceleration;
}