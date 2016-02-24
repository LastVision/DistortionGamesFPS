#pragma once
#include <GameEnum.h>
#include <Vector.h>

struct PhysEntityData
{
	ePhysics myPhysics;
	CU::Vector3<float> myPhysicsMin;
	CU::Vector3<float> myPhysicsMax;
};