#pragma once

#include "AnimationComponentData.h"
#include "GraphicsComponentData.h"

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	GraphicsComponentData myGraphicsData;

	std::string mySubType;
	eUnitType myUnitType;
};