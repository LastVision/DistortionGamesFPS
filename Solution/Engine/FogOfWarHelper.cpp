#include "stdafx.h"
#include <D3D11.h>
#include "FogOfWarHelper.h"
#include "RenderPlane.h"

namespace Prism
{
	FogOfWarHelper::FogOfWarHelper()
	{
		myTexture = new Texture();
		CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();
		myTexture->Init(windowSize.x, windowSize.y, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL
			, DXGI_FORMAT_R16G16B16A16_FLOAT);

	}


	FogOfWarHelper::~FogOfWarHelper()
	{
		SAFE_DELETE(myTexture);
	}

	void FogOfWarHelper::Render(Camera& aCamera, RenderPlane* aRenderPlane)
	{
		Engine* engine = Prism::Engine::GetInstance();
		//engine->DisableCulling();
		ID3D11RenderTargetView* renderTarget = myTexture->GetRenderTargetView();
		ID3D11DepthStencilView* depth = myTexture->GetDepthStencilView();
		float clearColor[4] = { 0, 0, 0, 0 };
		engine->GetContex()->ClearRenderTargetView(renderTarget, clearColor);
		engine->GetContex()->ClearDepthStencilView(depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		engine->GetContex()->OMSetRenderTargets(1, &renderTarget, depth);

		aRenderPlane->Render(aCamera, myOrientation);
	}


	Texture* FogOfWarHelper::GetTexture()
	{
		return myTexture;
	}

	void FogOfWarHelper::OnResize(float aWidth, float aHeight)
	{
		myTexture->Resize(aWidth, aHeight);
	}
}