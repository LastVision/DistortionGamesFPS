#include "stdafx.h"
#include "Behavior.h"


Behavior::Behavior(const Entity& anEntity)
	: myEntity(anEntity)
	, myDone(false)
	, myMaxSpeed(0.13f)
{
}


Behavior::~Behavior()
{
}
