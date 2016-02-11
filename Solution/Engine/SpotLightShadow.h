#pragma once

#include "SpotLight.h"



namespace Prism
{
	class Camera;
	class Texture;

	class SpotLightShadow : public SpotLight
	{
	public:
		SpotLightShadow(const CU::Matrix44<float>& aPlayerOrientation);
		~SpotLightShadow();

		void UpdateOrientation();

		Camera* GetCamera() const;
		void ClearTexture();

		Texture* GetTexture() const;
		void OnResize(float aWidth, float aHeight);
		CU::Matrix44<float> GetMVP() const;

	private:
		void operator=(SpotLightShadow&) = delete;

		Camera* myCamera;
		Texture* myTexture;
		CU::Matrix44<float> myBiasMatrix;
		const CU::Matrix44<float>& myPlayerOrientation;
	};


	inline Camera* SpotLightShadow::GetCamera() const
	{
		return myCamera;
	}

	inline Texture* SpotLightShadow::GetTexture() const
	{
		return myTexture;
	}
}