#pragma once
#include "ParticleData.h"

struct ID3D11InputLayout;
struct _D3DX11_TECHNIQUE_DESC;

namespace Prism
{
	class ParticleEmitterInstance;
	class Texture;
	class Effect;


	class ParticleEmitterData
	{
	
		friend class ParticleEmitterInstance;
		friend class ParticleDataContainer;

	public:
		ParticleEmitterData();
		~ParticleEmitterData();

		void LoadDataFile(const std::string& aFilePath);

	private:

		void CreateInputLayout();
		
		Texture*				myTexture;
		Effect*					myEffect;
		ID3D11InputLayout*		myInputLayout;
		_D3DX11_TECHNIQUE_DESC*	myTechniqueDesc;


		ParticleData			myData;
		std::string				myFileName;
		int						myMaxParticleAmount;

		std::string	myEffectName;
		std::string	myTextureName;

		CU::Vector3f myEmitterSize;

		float myEmissionAngle;
		float myAngleVariation;
		float myRotationDelta;
		float myEmitterLifeTime;
		float myEmissionRate;

		int	myParticlesPerEmitt;

		bool myUseEmitterLifeTime;
		bool myIsActiveAtStart;
		bool myIsHollow;
		bool myIsCircle;
		bool myIsAnimated;
	};
}