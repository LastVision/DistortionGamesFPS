#pragma once

struct ToolParticleData
{
	bool myIsAnimated;
	const char* myShaderPath;
	const char* myTexturePath;
	
	float myEmitterSizeX;
	float myEmitterSizeY;
	float myEmitterSizeZ;
	
	float myEmitterLifetime;
	
	int myParticlesPerEmitter;
	int myEmissionAngle;
	int myAngleVariation;
	int myParticleScale;
	int myParticleScaleDelta;
	
	int myRotationDelta;
	
	int myStartColorR;
	int myStartColorG;
	int myStartColorB;
	
	int myEndColorR;
	int myEndColorG;
	int myEndColorB;
	
	int myEmittsPerSecond;
	int myParticleLifetime;
	
	float myStartAlpha;
	float myMidAlpha;
};