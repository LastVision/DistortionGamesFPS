#include "stdafx.h"
#include "Camera.h"
#include <d3d11.h>
#include "Engine.h"
#include "SpotLightShadow.h"
#include "Texture.h"

namespace Prism
{
	SpotLightShadow::SpotLightShadow(const CU::Matrix44<float>& aPlayerOrientation)
		: myPlayerOrientation(aPlayerOrientation)
	{
		//myCamera = new Camera(myOrientation, 25.f, 1024, 1024);
		//myOrientation.myMatrix[0] = 0.728f;
		//myOrientation.myMatrix[1] = -0.568f;
		//myOrientation.myMatrix[2] = 0.383f;
		//myOrientation.myMatrix[3] = 0.f;
		//myOrientation.myMatrix[4] = 0.f;
		//myOrientation.myMatrix[5] = 0.559f;
		//myOrientation.myMatrix[6] = 0.829f;
		//myOrientation.myMatrix[7] = 0.f;
		//myOrientation.myMatrix[8] = -0.686f;
		//myOrientation.myMatrix[9] = -0.603f;
		//myOrientation.myMatrix[10] = 0.407f;
		//myOrientation.myMatrix[11] = 0.f;
		//myOrientation.myMatrix[12] = 335.f;
		//myOrientation.myMatrix[13] = 75.f;
		//myOrientation.myMatrix[14] = 100.f;
		//myOrientation.myMatrix[15] = 1.f;

		//myOrientation.myMatrix[0] = 0.898296595f;
		//myOrientation.myMatrix[1] = -0.300122738f;
		//myOrientation.myMatrix[2] = 0.320919782f;
		//myOrientation.myMatrix[3] = 0.f;
		//myOrientation.myMatrix[4] = 0.;
		//myOrientation.myMatrix[5] = 0.730376482f;
		//myOrientation.myMatrix[6] = 0.683044732f;
		//myOrientation.myMatrix[7] = 0.f;
		//myOrientation.myMatrix[8] = -0.439389527f;
		//myOrientation.myMatrix[9] = -0.613576829f;
		//myOrientation.myMatrix[10] = 0.656094670f;
		//myOrientation.myMatrix[11] = 0.f;
		//myOrientation.myMatrix[12] = 312.951294f;
		//myOrientation.myMatrix[13] = 236.149902f;
		//myOrientation.myMatrix[14] = -157.898010f;
		//myOrientation.myMatrix[15] = 1.f;

		SetDir(CU::Vector3<float>(0, 0, 1.f));
		SetRange(256.f);
		SetCone(0.3f);
		SetColor({ 0.6838235f, 0.7383367f, 1.f, 0.2f });
		
		SetPosition({ 0.f, 0.f, 0.f, 1.f });
		myOrientation = CU::Matrix44<float>::CreateRotateAroundX(3.14f / 3.f);
		myOrientation *= CU::Matrix44<float>::CreateRotateAroundZ(-3.14f / 4.f);
		myOrientation.SetPos({ 0.f, 75.f, 0.f, 1.f });
		Update();
		myCamera = new Camera(myOrientation);
		myCamera->InitShadowCamera(Prism::Engine::GetInstance()->GetWindowSize().x
			, Prism::Engine::GetInstance()->GetWindowSize().y);
		myTexture = new Texture();
		myTexture->InitAsDepthBuffer(1024.f* 2.f, 1024.f*2.f);

		myBiasMatrix.myMatrix[0] = 0.5;
		myBiasMatrix.myMatrix[5] = 0.5;
		myBiasMatrix.myMatrix[10] = 0.5;
		myBiasMatrix.SetPos({ 0.5, 0.5, 0.5, 1.f });

	}


	SpotLightShadow::~SpotLightShadow()
	{
		SAFE_DELETE(myCamera);
		SAFE_DELETE(myTexture);
	}

	void SpotLightShadow::UpdateOrientation()
	{
		myOrientation.SetPos(myPlayerOrientation.GetPos() + CU::Vector3<float>(90.f, 20.f, 0.f));
	}

	void SpotLightShadow::ClearTexture()
	{
		float color[4] = { 1.0f, 1.0f, 0.0f, 1.f };

		Engine::GetInstance()->GetContex()->ClearRenderTargetView(myTexture->GetRenderTargetView(), color);
		Engine::GetInstance()->GetContex()->ClearDepthStencilView(myTexture->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	void SpotLightShadow::OnResize(float aWidth, float aHeight)
	{
		myTexture->Resize(aWidth, aHeight);
	}

	CU::Matrix44<float> SpotLightShadow::GetMVP() const
	{
		return CU::InverseSimple(myOrientation) * myCamera->GetProjection();// *myBiasMatrix;
	}

}