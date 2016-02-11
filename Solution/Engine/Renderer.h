#pragma once

#include "EngineEnums.h"

#define MAX_NUMBER_OF_SCENES 2

struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct D3D11_VIEWPORT;
namespace Prism
{
	class FullScreenHelper;
	class Scene;
	class SpotLightShadow;
	class Texture;

	struct SceneData;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void ProcessShadow(SpotLightShadow* aShadowSpotLight, Scene* aScene);

		void BeginScene();
		void EndScene(int aEffect, Texture* aFogOfWarTexture);

		void FinalRender();

		void OnResize(float aWidth, float aHeight);

		Texture* GetWorldTexture();
		void SetRenderTargets(ID3D11RenderTargetView* aRenderTarget, ID3D11DepthStencilView* aDepthBuffer);

	private:
		struct SceneData
		{
			Texture* myScene;
			Texture* myFinished;
		};
		
		Engine* myEngine;
		SceneData mySceneData[MAX_NUMBER_OF_SCENES];

		FullScreenHelper* myFullScreenHelper;
		float myClearColor[4];
		int mySceneIndex;

		Texture* myFinalTexture;
		Texture* myCombineMiddleMan;

		D3D11_VIEWPORT* myShadowViewport;
	};

	inline Texture* Renderer::GetWorldTexture()
	{
		return mySceneData[1].myScene;
	}
}

