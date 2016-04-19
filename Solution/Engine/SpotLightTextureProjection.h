#pragma once

#include "SpotLight.h"

namespace Prism
{
	class Texture;
	class SpotLightTextureProjection : public SpotLight
	{
	public:
		SpotLightTextureProjection(unsigned int aGID, bool aIsForSpot);
		~SpotLightTextureProjection();

		void Init();

		void Render(const Camera& aCamera) override;
		void Update() override;

		const CU::Matrix44<float>& GetView() const;
		const CU::Matrix44<float>& GetProjection() const;
		const CU::Matrix44<float>& GetOrientation() const;

	private:
		CU::Matrix44<float> myProjectionMatrix;
		CU::Matrix44<float> myViewMatrix;
		Texture* myTexture;
	};

}