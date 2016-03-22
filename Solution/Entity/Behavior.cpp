#include "stdafx.h"
#include "Behavior.h"
#include "AIComponentData.h"

Behavior::Behavior(const Entity& anEntity, const AIComponentData& aData)
	: myEntity(anEntity)
	, myData(aData)
	, myDone(false)
	, myMaxSpeed(aData.mySpeed)
{
}


Behavior::~Behavior()
{
}
