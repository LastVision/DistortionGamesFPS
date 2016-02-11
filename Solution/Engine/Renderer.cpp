#include "stdafx.h"

#include "FullScreenHelper.h"
#include "Texture.h"
#include "Renderer.h"
#include "Scene.h"
#include "SpotLightShadow.h"
#include <D3DX11.h>

namespace Prism
{
	Renderer::Renderer()
	{
		myEngine = Engine::GetInstance();

		CU::Vector2<float> screenSize(static_cast<float>(Engine::GetInstance()->GetWindowSize().x)
			, static_cast<float>(Engine::GetInstance()->GetWindowSize().y));

		for (int i = 0; i < MAX_NUMBER_OF_SCENES; ++i)
		{
			mySceneData[i].myScene = new Texture();
			mySceneData[i].myScene->Init(screenSize.x, screenSize.y
				, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL
				, DXGI_FORMAT_R16G16B16A16_FLOAT);

			mySceneData[i].myFinished = new Texture();
			mySceneData[i].myFinished->Init(screenSize.x, screenSize.y
				, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL
				, DXGI_FORMAT_R16G16B16A16_FLOAT);
		}

		myFinalTexture = new Texture();
		myFinalTexture->Init(screenSize.x, screenSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL
			, DXGI_FORMAT_R16G16B16A16_FLOAT);

		myCombineMiddleMan = new Texture();
		myCombineMiddleMan->Init(screenSize.x, screenSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL
			, DXGI_FORMAT_R16G16B16A16_FLOAT);

		myFullScreenHelper = new FullScreenHelper();

		myClearColor[0] = 0.f;
		myClearColor[1] = 0.f;
		myClearColor[2] = 0.f;
		myClearColor[3] = 0.f;

		mySceneIndex = 0;

		myShadowViewport = new D3D11_VIEWPORT[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
		myShadowViewport->Height = 1024 * 2;
		myShadowViewport->Width = 1024 * 2;
		myShadowViewport->TopLeftX = 0;
		myShadowViewport->TopLeftY = 0;
		myShadowViewport->MinDepth = 0;
		myShadowViewport->MaxDepth = 1;
	}
	
	
	Renderer::~Renderer()
	{
		SAFE_DELETE(myShadowViewport);
		SAFE_DELETE(myCombineMiddleMan);
		SAFE_DELETE(myFinalTexture);
		for (int i = 0; i < MAX_NUMBER_OF_SCENES; ++i)
		{
			SAFE_DELETE(mySceneData[i].myScene);
			SAFE_DELETE(mySceneData[i].myFinished);
		}
		SAFE_DELETE(myFullScreenHelper);
	}

	void Renderer::ProcessShadow(SpotLightShadow* aShadowSpotLight, Scene* aScene)
	{
		aShadowSpotLight->ClearTexture();

		UINT nbrOfVp = 1;
		D3D11_VIEWPORT oldVp;
		Engine::GetInstance()->GetContex()->RSGetViewports(&nbrOfVp, &oldVp);


		Engine::GetInstance()->GetContex()->RSSetViewports(1, myShadowViewport);
		

		ID3D11RenderTargetView* originalRenderTargetView;
		ID3D11DepthStencilView* originalDepthStencilView;
		Engine::GetInstance()->GetContex()->OMGetRenderTargets(1, &originalRenderTargetView, &originalDepthStencilView);

		ID3D11RenderTargetView* view = aShadowSpotLight->GetTexture()->GetRenderTargetView();

		Engine::GetInstance()->GetContex()->OMSetRenderTargets(1, &view, aShadowSpotLight->GetTexture()->GetDepthStencilView());
		const Camera* oldCamera = aScene->GetCamera();
		aScene->SetViewCamera(*oldCamera);
		aScene->SetCamera(*aShadowSpotLight->GetCamera());

		aScene->Render(false);
		//Prism::Engine::GetInstance()->DisableZBuffer();
		//aScene->RenderEmitters();
		//Prism::Engine::GetInstance()->EnableZBuffer();

		Engine::GetInstance()->GetContex()->OMSetRenderTargets(1, &originalRenderTargetView, originalDepthStencilView);

		Engine::GetInstance()->GetContex()->RSSetViewports(1, &oldVp);
		aScene->SetCamera(*oldCamera);
	}

	void Renderer::BeginScene()
	{
		DL_ASSERT_EXP(mySceneIndex < MAX_NUMBER_OF_SCENES, "Tried to Begin to many Scenes");

		SceneData& data = mySceneData[mySceneIndex];

		ID3D11RenderTargetView* renderTarget = data.myScene->GetRenderTargetView();
		ID3D11DepthStencilView* depth = data.myScene->GetDepthStencilView();
		myEngine->GetContex()->ClearRenderTargetView(renderTarget, myClearColor);
		myEngine->GetContex()->ClearDepthStencilView(depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		myEngine->GetContex()->OMSetRenderTargets(1, &renderTarget, depth);
	}

	void Renderer::EndScene(int aEffect, Texture* aFogOfWarTexture)
	{
		DL_ASSERT_EXP(mySceneIndex < MAX_NUMBER_OF_SCENES, "Tried to Begin to many Scenes");

		SceneData& data = mySceneData[mySceneIndex];

		ID3D11RenderTargetView* renderTarget = data.myFinished->GetRenderTargetView();
		myEngine->GetContex()->ClearRenderTargetView(renderTarget, myClearColor);

		myFullScreenHelper->Process(data.myScene, data.myFinished, aEffect, aFogOfWarTexture);

		++mySceneIndex;
	}

	void Renderer::FinalRender()
	{
		myEngine->GetContex()->ClearRenderTargetView(myFinalTexture->GetRenderTargetView(), myClearColor);
		
		Engine::GetInstance()->DisableZBuffer();

		myFullScreenHelper->CombineTextures(mySceneData[0].myFinished, mySceneData[0].myScene
			, mySceneData[1].myFinished, mySceneData[1].myScene, myFinalTexture);

		
		myFullScreenHelper->RenderToScreen(myFinalTexture, mySceneData[1].myScene);

		mySceneIndex = 0;

		Engine::GetInstance()->EnableZBuffer();
	}

	void Renderer::OnResize(float aWidth, float aHeight)
	{
		for (int i = 0; i < MAX_NUMBER_OF_SCENES; ++i)
		{
			mySceneData[i].myScene->Resize(aWidth, aHeight);

			mySceneData[i].myFinished->Resize(aWidth, aHeight);
		}

		myFinalTexture->Resize(aWidth, aHeight);

		myCombineMiddleMan->Resize(aWidth, aHeight);


		myFullScreenHelper->OnResize(aWidth, aHeight);
	}

	void Renderer::SetRenderTargets(ID3D11RenderTargetView* aRenderTarget, ID3D11DepthStencilView* aDepthBuffer)
	{
		ID3D11RenderTargetView* target = aRenderTarget;
		ID3D11DepthStencilView* depth = aDepthBuffer;
		Engine::GetInstance()->GetContex()->OMSetRenderTargets(1, &target, depth);
	}
}
