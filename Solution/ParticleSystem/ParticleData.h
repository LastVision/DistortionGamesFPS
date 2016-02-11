#pragma once
#include <Vector.h>

struct ParticleData
{
	ParticleData()
		: myLifeTime(0.f)
		, myMaxStartSize(0.f)
		, myMinStartSize(0.f)
		, mySizeDelta(0.f)
		, myStartAlpha(0.f)
		, myAlphaDelta(0.f)
	{}
	CU::Vector3f myMaxVelocity;
	CU::Vector3f myMinVelocity;

	CU::Vector3f myStartColor;
	CU::Vector3f myEndColor;

	float myLifeTime;
	
	float myMaxStartSize;
	float myMinStartSize;

	float mySizeDelta;

	float myStartAlpha;
	float myAlphaDelta;

};

struct GraphicalParticle
{
	GraphicalParticle()
		: myAlpha(0.0f)
		, mySize(0.0f)
		, myLifeTime(0.0f)
		, myRotation(0.0f)
	{
	}

	~GraphicalParticle()
	{
	}

	CU::Vector3f myPosition;
	CU::Vector3f myColor;
	float myAlpha;
	float mySize;
	float myLifeTime;
	float myRotation;
};

struct LogicalParticle
{
	LogicalParticle()
		: mySpeed(0.0f)
		, myIsAlive(false)
		, myRotation(0.0f)
		, myRotationDelta(0.0f)
	{
	}
	~LogicalParticle()
	{
	}
	bool myIsAlive;
	CU::Vector3f myVelocity;
	float mySpeed;
	float myRotation;
	float myRotationDelta;

};
