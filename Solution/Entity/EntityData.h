#pragma once

#include "AnimationComponentData.h"
#include "GraphicsComponentData.h"
#include "FirstPersonRenderComponentData.h"
#include "PhysicsComponentData.h"
#include "HealthComponentData.h"
#include "InputComponentData.h"
#include "NetworkComponentData.h"
#include "GrenadeComponentData.h"
#include "ShootingComponentData.h"
#include "TriggerComponentData.h"
#include "UpgradeComponentData.h"

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	FirstPersonRenderComponentData myFirstPersonRenderData;
	GraphicsComponentData myGraphicsData;
	HealthComponentData myHealthData;
	InputComponentData myInputData;
	NetworkComponentData myNetworkData;
	GrenadeComponentData myProjecileData;
	PhysicsComponentData myPhysicsData;
	ShootingComponentData myShootingData;
	TriggerComponentData myTriggerData;
	UpgradeComponentData myUpgradeData;

	std::string mySubType;
};