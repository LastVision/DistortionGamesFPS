#pragma once
#include <string>
#include <unordered_map>

namespace Prism
{
	class StreakEmitterData;

	class StreakDataContainer
	{

	public:
		static StreakDataContainer* GetInstance();
		static void Destroy();

		StreakEmitterData* GetStreakData(const char* aFilePath);

	private:
		StreakDataContainer(){};
		~StreakDataContainer();
		void LoadParticleData(const char* aFilePath);

		std::unordered_map<const char*, StreakEmitterData*> myParticleData;

		static StreakDataContainer* myInstance;
	};
};