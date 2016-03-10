#pragma once
#include <GameEnum.h>
#include <Vector.h>

struct PhysicsComponentData
{
	bool myExistsInEntity = false;
	ePhysics myPhysicsType;
	CU::Vector3<float> myPhysicsMin;
	CU::Vector3<float> myPhysicsMax;
};