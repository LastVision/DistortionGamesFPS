#include "stdafx.h"
#include "ArriveBehavior.h"
#include "BlendedBehavior.h"
#include "EvadeBehavior.h"

BlendedBehavior::BlendedBehavior(const Entity& anEntity)
	: Behavior(anEntity)
	, myBehaviors(8)
	, myPreviousAcceleration(0, 0)
	, myMaxAcceleration(100000.f)
{
	myBehaviors.Add(new ArriveBehavior(myEntity));
	myBehaviors.Add(new EvadeBehavior(myEntity));
}


BlendedBehavior::~BlendedBehavior()
{
	myBehaviors.DeleteAll();
}

const CU::Vector2<float>& BlendedBehavior::Update(float aDelta)
{
	myAcceleration.x = 0;
	myAcceleration.y = 0;

	for (int i = 0; i < myBehaviors.Size(); ++i)
	{
		myAcceleration += myBehaviors[i]->Update(aDelta);
	}
	//DL_DEBUG("length: %f", CU::Length2(myPreviousAcceleration + myAcceleration));

	/*if (CU::Length2(myPreviousAcceleration + myAcceleration) < 1.f)
	{
		myDone = true;
	}*/

	if (CU::Length2(myAcceleration) > myMaxAcceleration*myMaxAcceleration)
	{
		CU::Normalize(myAcceleration);
		myAcceleration *= myMaxAcceleration;
	}

	myPreviousAcceleration = myAcceleration;

	return myAcceleration;
}

void BlendedBehavior::SetTarget(const CU::Vector2<float>& aTargetPosition)
{
	myTarget = aTargetPosition;
	myPreviousAcceleration = CU::Vector2<float>(0, 0);
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