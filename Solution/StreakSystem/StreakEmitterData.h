#pragma once
#include "StreakData.h"
#include <D3D11.h>
#include <string>
#include <Vector.h>

namespace Prism
{
	class Texture;
	class Effect;
	class StreakEmitterInstance;

	class StreakEmitterData
	{
		friend StreakEmitterInstance;
	public:
		StreakEmitterData();
		~StreakEmitterData();
		bool LoadDataFile(const std::string& aFileName);

	private:
		bool CreateInputLayout();
		Effect* myEffect;
		std::string myEffectName;
		CU::Vector3f myEmissionExtents;
		float myEmissionLifeTime;
		float myEmissionRate;
		float myEmissionRateDelta;
		float myEmissionSpeed;
		CU::Vector3f myEmissionVelocityDelta;
		int myMaxNrOfStreaks;
		CU::Vector3f myMaxRotation;
		CU::Vector3f myMaxVelocity;
		CU::Vector3f myMinRotation;
		CU::Vector3f myMinVelocity;
		StreakData myStreakData;
		ID3D11InputLayout* myStreakInputLayout;
		float myMinRotationDelta;
		float myMaxRotationDelta;
		Texture* myTexture;
		std::string myTextureName;

		CU::Vector3f myGravity;
	};

}