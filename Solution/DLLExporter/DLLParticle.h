#pragma once
#include <string>
#include <Matrix.h>

namespace Prism
{
	class Camera;
	class ParticleEmitterData;
	class ParticleEmitterInstance;
}

class DLLParticle
{
public:
	DLLParticle();
	~DLLParticle();

	void LoadParticle(std::string& aParticleFile);
	void Update(float aDeltaTime);
	void Render(Prism::Camera* aCamera);
private:
	void ReLoadParticle();

	bool myIsLoaded;

	CU::Matrix44<float> myOrientation;

	Prism::ParticleEmitterData* myParticleData;
	Prism::ParticleEmitterInstance* myCurrentParticle;

	std::string myParticleFile;
};

