#pragma once

namespace Prism
{
	class RenderPlane;
	class Texture;

	class FogOfWarHelper
	{
	public:
		FogOfWarHelper();
		~FogOfWarHelper();
		void Render(Camera& aCamera, RenderPlane* aRenderPlane);

		Texture* GetTexture();
		void OnResize(float aWidth, float aHeight);

	private:

		Texture* myTexture;
		CU::Matrix44<float> myOrientation;
	};
}