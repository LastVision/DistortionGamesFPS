#pragma once
#include <string>
#include <unordered_map>

namespace Prism
{
	class Camera;
	class ParticleEmitterData;
	
	class ParticleDataContainer
	{
	public:
		static ParticleDataContainer* GetInstance();
		static void Destroy();

		ParticleEmitterData*	GetParticleData(const std::string& aFilePath);
		void SetGPUData(const Camera& aCamera);

	private:
		ParticleDataContainer(){};
		~ParticleDataContainer();
		void LoadParticleData(const std::string& aFilePath);

		std::unordered_map<std::string, ParticleEmitterData*> myParticleData;

		static ParticleDataContainer* myInstance;
	};
};