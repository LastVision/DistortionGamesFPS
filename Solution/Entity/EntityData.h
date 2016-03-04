#pragma once

#include "AnimationComponentData.h"
#include "GraphicsComponentData.h"
#include "HealthComponentData.h"
#include "ProjectileComponentData.h"
#include "TriggerComponentData.h"
#include <PhysEntityData.h>

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	GraphicsComponentData myGraphicsData;
	ProjectileComponentData myProjecileData;
	PhysEntityData myPhysData;
	HealthComponentData myHealthData;
	TriggerComponentData myTriggerData;

	std::string mySubType;
};