#pragma once
#include <Vector.h>
#include <Matrix.h>

namespace Prism
{
	class Camera;
	class DeferredRenderer;
	class Texture;
	class Scene;

	struct SHNode
	{
		float myData[3][9];
		CU::Vector4<float> position;
	};

	struct SHGridNode
	{
		CU::Vector4<float> cAr;
		CU::Vector4<float> cAg;
		CU::Vector4<float> cAb;
		CU::Vector4<float> cBr;
		CU::Vector4<float> cBg;
		CU::Vector4<float> cBb;
		CU::Vector4<float> cC;
	};

	struct SHTextures
	{
		~SHTextures();

		Texture* cAr;
		Texture* cAg;
		Texture* cAb;
		Texture* cBr;
		Texture* cBg;
		Texture* cBb;
		Texture* cC;
	};

	class CubeMapGenerator
	{
	public:
		CubeMapGenerator();
		~CubeMapGenerator();

		void GenerateSHTextures(DeferredRenderer* aRenderer, Scene* aScene, SHTextures& someTextures
			, const CU::Vector3<float>& aGridSize, const CU::Vector3<float>& aOffset, float aNodeSize
			, const std::string& aName);
		Texture* GenerateCubeMap(DeferredRenderer* aRenderer, Scene* aScene, const CU::Vector3<float> aCameraPosition
			, const CU::Vector2<float>& aTextureSize, bool aUseAmbientPass, const std::string& aFileName);
		
		void OnResize(float aWidth, float aHeight);

	private:

		SHNode GenerateSHNode(Texture* aTexture, const CU::Vector3<float> aCameraPosition);
		SHGridNode GetSHGridNode(const SHNode& aNode);

		Camera* myCamera;
		CU::Matrix44<float> myCameraOrientations[6];
		CU::Matrix44<float> myCurrentCameraOrientation;

		int mySH_GRID_X;
		int mySH_GRID_Y;
		int mySH_GRID_Z;
	};
}