#pragma once

#include "AnimationComponentData.h"
#include "GraphicsComponentData.h"
#include "FirstPersonRenderComponentData.h"
#include "PhysicsComponentData.h"
#include "HealthComponentData.h"
#include "InputComponentData.h"
#include "NetworkComponentData.h"
#include "ProjectileComponentData.h"
#include "ShootingComponentData.h"
#include "TriggerComponentData.h"


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
	PhysicsComponentData myPhysicsData;
	ShootingComponentData myShootingData;
	TriggerComponentData myTriggerData;

	std::string mySubType;
};