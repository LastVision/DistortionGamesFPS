#pragma once

class GC
{
public:
	static bool GenerateLightData;
	static bool GenerateCubeMap;
	static int SHNodeSize;

	static bool EnableCheapAmbient;
	static bool EnableVSync;

	static int Difficulty;
	static float DamageMultiplier[3];
	static float SpawnIntervalMultiplier[3];

	static volatile bool PlayerAlive;
};