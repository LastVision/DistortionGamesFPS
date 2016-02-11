#include "stdafx.h"
#include "Behavior.h"


Behavior::Behavior(const Entity& anEntity)
	: myEntity(anEntity)
	, myDone(false)
{
}


Behavior::~Behavior()
{
}
