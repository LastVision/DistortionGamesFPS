#pragma once

#include "AnimationComponentData.h"
#include "FirstPersonRenderComponentData.h"
#include "GraphicsComponentData.h"
#include "HealthComponentData.h"
#include "InputComponentData.h"
#include "NetworkComponentData.h"
#include "ProjectileComponentData.h"
#include "ShootingComponentData.h"
#include "TriggerComponentData.h"
#include <PhysEntityData.h>

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	FirstPersonRenderComponentData myFirstPersonRenderData;
	GraphicsComponentData myGraphicsData;
	HealthComponentData myHealthData;
	InputComponentData myInputData;
	NetworkComponentData myNetworkData;
	ProjectileComponentData myProjecileData;
	PhysEntityData myPhysData;
	ShootingComponentData myShootingData;
	TriggerComponentData myTriggerData;

	std::string mySubType;
};