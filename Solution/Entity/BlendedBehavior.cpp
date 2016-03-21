#include "stdafx.h"
#include "ArriveBehavior.h"
#include "BlendedBehavior.h"
#include "EvadeBehavior.h"

BlendedBehavior::BlendedBehavior(const Entity& anEntity, const AIComponentData& aData)
	: Behavior(anEntity, aData)
	, myBehaviors(8)
	, myMaxAcceleration(100000.f)
{
	myBehaviors.Add(new ArriveBehavior(myEntity, myData));
	//myBehaviors.Add(new EvadeBehavior(myEntity));
}


BlendedBehavior::~BlendedBehavior()
{
	myBehaviors.DeleteAll();
}

const CU::Vector3<float>& BlendedBehavior::Update(float aDelta)
{
	myVelocity.x = 0;
	myVelocity.z = 0;

	for (int i = 0; i < myBehaviors.Size(); ++i)
	{
		myVelocity += myBehaviors[i]->Update(aDelta);
	}
	//DL_DEBUG("length: %f", CU::Length2(myPreviousAcceleration + myAcceleration));

	/*if (CU::Length2(myPreviousAcceleration + myAcceleration) < 1.f)
	{
		myDone = true;
	}*/

	if (CU::Length2(myVelocity) > myMaxSpeed * myMaxSpeed)
	{
		CU::Normalize(myVelocity);
		myVelocity *= myMaxSpeed;
	}

	//myPreviousAcceleration = myAcceleration;

	return myVelocity;
}

void BlendedBehavior::SetTarget(const CU::Vector3<float>& aTargetPosition)
{
	myTarget = aTargetPosition;
	//myPreviousAcceleration = CU::Vector3<float>(0, 0);
	myDone = false;
	for (int i = 0; i < myBehaviors.Size(); ++i)
	{
		myBehaviors[i]->SetTarget(aTargetPosition);
	}
}

bool BlendedBehavior::GetDone() const
{
	if (myDone == true)
	{
		return myDone;
	}
	for (int i = 0; i < myBehaviors.Size(); ++i)
	{
		if (myBehaviors[i]->GetDone() == true)
		{
			return true;
		}
	}
	return false;
}