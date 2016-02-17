#pragma once
#include <string>
#include <unordered_map>

namespace Prism
{
	class Effect;
	class SpotLightShadow;

	class EffectContainer
	{
	public:
		static EffectContainer* GetInstance();
		static void Destroy();

		Effect* GetEffect(const std::string& aFilePath);

		void Update(const float aDeltaTime);
		void SetCubeMap(const std::string& aFilePath);
		void SetShadowDepth(SpotLightShadow* aShadowSpotLight);

	private:
		EffectContainer() : myEffectArrays(64){};
		~EffectContainer();

		void LoadEffect(const std::string& aFilePath);
		void VerifyShader(const std::string& aFilePath);

		std::string GetCSOPath(const std::string& aFXPath);

		std::unordered_map<std::string, Effect*> myEffects;
		CU::GrowingArray<Effect*> myEffectArrays;
		std::string myCubeMap;

		static EffectContainer* myInstance;
	};
}