#pragma once

#include "AnimationComponentData.h"
#include "GraphicsComponentData.h"
#include "HealthComponentData.h"
#include "NetworkComponentData.h"
#include "ProjectileComponentData.h"
#include "TriggerComponentData.h"
#include <PhysEntityData.h>

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	GraphicsComponentData myGraphicsData;
	HealthComponentData myHealthData;
	NetworkComponentData myNetworkData;
	ProjectileComponentData myProjecileData;
	PhysEntityData myPhysData;
	TriggerComponentData myTriggerData;

	std::string mySubType;
};