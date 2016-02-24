#pragma once

#include "AnimationComponentData.h"
#include "GraphicsComponentData.h"
#include "ProjectileComponentData.h"
#include <PhysEntityData.h>

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	GraphicsComponentData myGraphicsData;
	ProjectileComponentData myProjecileData;
	PhysEntityData myPhysData;

	std::string mySubType;
};