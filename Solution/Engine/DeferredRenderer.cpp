#include "stdafx.h"

#include "Camera.h"
#include <d3dx11effect.h>
#include "DeferredRenderer.h"
#include "IndexBufferWrapper.h"
#include "Texture.h"
#include "Scene.h"
#include "PointLight.h"

namespace Prism
{
	DeferredRenderer::DeferredRenderer()
	{
		CU::Vector2<float> windowSize = Engine::GetInstance()->GetWindowSize();

		myDepthStencilTexture = new Texture();
		myDepthStencilTexture->InitAsDepthBuffer(windowSize.x, windowSize.y);

		myClearColor[0] = 0.f;
		myClearColor[1] = 0.f;
		myClearColor[2] = 0.f;
		myClearColor[3] = 0.f;

		myRenderToScreenData.myEffect = 
			EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_render_to_texture.fx");
		myRenderToScreenData.mySource =
			myRenderToScreenData.myEffect->GetEffect()->GetVariableByName("DiffuseTexture")->AsShaderResource();

		SetupAmbientData();
		SetupLightData();
		SetupGBufferData();

		InitFullscreenQuad();
	}


	DeferredRenderer::~DeferredRenderer()
	{
		SAFE_DELETE(myGBufferData.myAlbedoTexture);
		SAFE_DELETE(myGBufferData.myNormalTexture);
		SAFE_DELETE(myGBufferData.myDepthTexture);
		SAFE_DELETE(myGBufferData.myMetalnessTexture);
		SAFE_DELETE(myGBufferData.myAmbientOcclusionTexture);
		SAFE_DELETE(myGBufferData.myRoughnessTexture);
	}

	void DeferredRenderer::Render(Scene* aScene)
	{
		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();
		context->ClearRenderTargetView(myGBufferData.myAlbedoTexture->GetRenderTargetView(), myClearColor);
		context->ClearRenderTargetView(myGBufferData.myNormalTexture->GetRenderTargetView(), myClearColor);
		context->ClearRenderTargetView(myGBufferData.myDepthTexture->GetRenderTargetView(), myClearColor);
		context->ClearRenderTargetView(myGBufferData.myMetalnessTexture->GetRenderTargetView(), myClearColor);
		context->ClearRenderTargetView(myGBufferData.myAmbientOcclusionTexture->GetRenderTargetView(), myClearColor);
		context->ClearRenderTargetView(myGBufferData.myRoughnessTexture->GetRenderTargetView(), myClearColor);

		context->ClearDepthStencilView(myDepthStencilTexture->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* targets[6];
		targets[0] = myGBufferData.myAlbedoTexture->GetRenderTargetView();
		targets[1] = myGBufferData.myNormalTexture->GetRenderTargetView();
		targets[2] = myGBufferData.myDepthTexture->GetRenderTargetView();
		targets[3] = myGBufferData.myMetalnessTexture->GetRenderTargetView();
		targets[4] = myGBufferData.myAmbientOcclusionTexture->GetRenderTargetView();
		targets[5] = myGBufferData.myRoughnessTexture->GetRenderTargetView();

		context->OMSetRenderTargets(6, targets
			, myDepthStencilTexture->GetDepthStencilView());

		aScene->Render();

		ActivateBuffers();

		RenderDeferred(aScene);
	}

	void DeferredRenderer::OnResize(float aWidth, float aHeight)
	{
		myGBufferData.myAlbedoTexture->Resize(aWidth, aHeight);
		myGBufferData.myNormalTexture->Resize(aWidth, aHeight);
		myGBufferData.myDepthTexture->Resize(aWidth, aHeight);
		myGBufferData.myMetalnessTexture->Resize(aWidth, aHeight);
		myGBufferData.myAmbientOcclusionTexture->Resize(aWidth, aHeight);
		myGBufferData.myRoughnessTexture->Resize(aWidth, aHeight);
		myDepthStencilTexture->Resize(aWidth, aHeight);
	}

	void DeferredRenderer::InitFullscreenQuad()
	{
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		D3DX11_PASS_DESC passDesc;
		myRenderToScreenData.myEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateInputLayout(vertexDesc
			, ARRAYSIZE(vertexDesc), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &myVertexLayout);
		if (FAILED(hr) != S_OK)
		{
			DL_MESSAGE_BOX("Failed to CreateInputLayout", "Model2D::Init", MB_ICONWARNING);
		}


		InitVertexBuffer(sizeof(VertexPosUV), D3D11_USAGE_IMMUTABLE, 0);
		InitIndexBuffer();

		CreateVertices();
	}

	void DeferredRenderer::CreateVertices()
	{
		CU::GrowingArray<VertexPosUV> vertices;
		vertices.Init(4);
		vertices.Add({ { -1.f, -1.f, 0.f }, { 0.f, 1.f } }); //topleft
		vertices.Add({ { 1.f, -1.f, 0.f }, { 1.f, 1.f } }); //topright
		vertices.Add({ { -1.f, 1.f, 0.f }, { 0.f, 0.f } }); //bottomleft
		vertices.Add({ { 1.f, 1.f, 0.f }, { 1.f, 0.f } }); //bottomright

		CU::GrowingArray<int> indices;
		indices.Init(6);

		indices.Add(0);
		indices.Add(2);
		indices.Add(1);

		indices.Add(1);
		indices.Add(2);
		indices.Add(3);

		SetupVertexBuffer(vertices.Size(), sizeof(VertexPosUV), reinterpret_cast<char*>(&vertices[0]), "DeferredRenderer::VertexBuffer");
		SetupIndexBuffer(indices.Size(), reinterpret_cast<char*>(&indices[0]), "DeferredRenderer::IndexBuffer");
	}

	void DeferredRenderer::ActivateBuffers()
	{
		ID3D11Buffer* buf = myVertexBuffer->myVertexBuffer;
		Engine::GetInstance()->GetContex()->IASetInputLayout(myVertexLayout);
		Engine::GetInstance()->GetContex()->IASetVertexBuffers(myVertexBuffer->myStartSlot,
			myVertexBuffer->myNumberOfBuffers, &buf,
			&myVertexBuffer->myStride, &myVertexBuffer->myByteOffset);
		Engine::GetInstance()->GetContex()->IASetIndexBuffer(myIndexBuffer->myIndexBuffer,
			myIndexBuffer->myIndexBufferFormat, myIndexBuffer->myByteOffset);
		Engine::GetInstance()->GetContex()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void DeferredRenderer::Render(Effect* aEffect)
	{
		D3DX11_TECHNIQUE_DESC techDesc;
		aEffect->GetTechnique()->GetDesc(&techDesc);
		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			aEffect->GetTechnique()->GetPassByIndex(i)->Apply(0, Engine::GetInstance()->GetContex());
			Engine::GetInstance()->GetContex()->DrawIndexed(6, 0, 0);
		}
	}

	void DeferredRenderer::RenderDeferred(Scene* aScene)
	{
		Engine::GetInstance()->RestoreViewPort();
		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();

		ID3D11RenderTargetView* backbuffer = Engine::GetInstance()->GetBackbuffer();
		context->ClearRenderTargetView(backbuffer, myClearColor);
		context->ClearDepthStencilView(Engine::GetInstance()->GetDepthView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		context->OMSetRenderTargets(1, &backbuffer
			, Engine::GetInstance()->GetDepthView());

		myAmbientPass.myAlbedo->SetResource(myGBufferData.myAlbedoTexture->GetShaderView());
		myAmbientPass.myNormal->SetResource(myGBufferData.myNormalTexture->GetShaderView());
		myAmbientPass.myDepth->SetResource(myGBufferData.myDepthTexture->GetShaderView());
		//myAmbientPass.myCubemap->SetResource(myCubemap->GetShaderView());

		Render(myAmbientPass.myEffect);

		myAmbientPass.myAlbedo->SetResource(nullptr);
		myAmbientPass.myNormal->SetResource(nullptr);
		myAmbientPass.myDepth->SetResource(nullptr);
		myAmbientPass.myCubemap->SetResource(nullptr);

		context->OMSetRenderTargets(1, &backbuffer
			, myDepthStencilTexture->GetDepthStencilView());
#ifdef USE_LIGHTS

		aScene->UpdateLights();
		RenderPointLights(aScene);
#endif
	}

	void DeferredRenderer::RenderPointLights(Scene* aScene)
	{
		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();
		const Camera& camera = *aScene->GetCamera();

		myLightPass.myAlbedo->SetResource(myGBufferData.myAlbedoTexture->GetShaderView());
		myLightPass.myNormal->SetResource(myGBufferData.myNormalTexture->GetShaderView());
		myLightPass.myDepth->SetResource(myGBufferData.myDepthTexture->GetShaderView());
		myLightPass.myMetalness->SetResource(myGBufferData.myMetalnessTexture->GetShaderView());
		myLightPass.myAmbientOcclusion->SetResource(myGBufferData.myAmbientOcclusionTexture->GetShaderView());
		myLightPass.myRoughness->SetResource(myGBufferData.myRoughnessTexture->GetShaderView());

		myLightPass.myInvertedProjection->SetMatrix(&CU::InverseReal(aScene->GetCamera()->GetProjection()).myMatrix[0]);
		myLightPass.myNotInvertedView->SetMatrix(&aScene->GetCamera()->GetOrientation().myMatrix[0]);


		const CU::GrowingArray<PointLight*>& lights = aScene->GetPointLights();

		Engine::GetInstance()->SetRasterizeState(eRasterizer::NO_CULLING);
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::READ_NO_WRITE);
		for (int i = 0; i < lights.Size(); ++i)
		{
			myLightPass.myPointLightVariable->SetRawValue(&lights[i]->GetLightData(), 0, sizeof(PointLightData));
			lights[i]->Render(camera);
		}
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);

		myLightPass.myAlbedo->SetResource(nullptr);
		myLightPass.myNormal->SetResource(nullptr);
		myLightPass.myDepth->SetResource(nullptr);
		myLightPass.myMetalness->SetResource(nullptr);
		myLightPass.myAmbientOcclusion->SetResource(nullptr);
		myLightPass.myRoughness->SetResource(nullptr);
	}

	void DeferredRenderer::SetupAmbientData()
	{
		myAmbientPass.myEffect = EffectContainer::GetInstance()->GetEffect(
			"Data/Resource/Shader/S_effect_deferred_ambient.fx");
		myAmbientPass.myAlbedo
			= myAmbientPass.myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myAmbientPass.myNormal
			= myAmbientPass.myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myAmbientPass.myDepth
			= myAmbientPass.myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();
		myAmbientPass.myCubemap
			= myAmbientPass.myEffect->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();
		myAmbientPass.myAmbientMultiplier
			= myAmbientPass.myEffect->GetEffect()->GetVariableByName("AmbientMultiplier")->AsScalar();

#ifdef USE_LIGHTS
		myAmbientPass.myAmbientMultiplier->SetFloat(0.05f);
#else
		myAmbientPass.myAmbientMultiplier->SetFloat(1.f);
#endif
	}

	void DeferredRenderer::SetupLightData()
	{
		myLightPass.myEffect = EffectContainer::GetInstance()->GetEffect(
			"Data/Resource/Shader/S_effect_deferred_light_mesh.fx");
		myLightPass.myAlbedo
			= myLightPass.myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myLightPass.myNormal
			= myLightPass.myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myLightPass.myDepth
			= myLightPass.myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();
		myLightPass.myMetalness
			= myLightPass.myEffect->GetEffect()->GetVariableByName("MetalnessTexture")->AsShaderResource();
		myLightPass.myAmbientOcclusion
			= myLightPass.myEffect->GetEffect()->GetVariableByName("AOTexture")->AsShaderResource();
		myLightPass.myRoughness
			= myLightPass.myEffect->GetEffect()->GetVariableByName("RoughnessTexture")->AsShaderResource();

		myLightPass.myPointLightVariable
			= myLightPass.myEffect->GetEffect()->GetVariableByName("PointLights");

		myLightPass.myInvertedProjection = myLightPass.myEffect->GetEffect()->GetVariableByName("InvertedProjection")->AsMatrix();
		myLightPass.myNotInvertedView = myLightPass.myEffect->GetEffect()->GetVariableByName("NotInvertedView")->AsMatrix();
	}

	void DeferredRenderer::SetupGBufferData()
	{
		CU::Vector2<float> windowSize = Engine::GetInstance()->GetWindowSize();
		myGBufferData.myAlbedoTexture = new Texture();
		myGBufferData.myAlbedoTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R8G8B8A8_UNORM);

		myGBufferData.myNormalTexture = new Texture();
		myGBufferData.myNormalTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R8G8B8A8_UNORM);

		myGBufferData.myDepthTexture = new Texture();
		myGBufferData.myDepthTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R32G32B32A32_FLOAT);

		myGBufferData.myMetalnessTexture = new Texture();
		myGBufferData.myMetalnessTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R32G32B32A32_FLOAT);

		myGBufferData.myAmbientOcclusionTexture = new Texture();
		myGBufferData.myAmbientOcclusionTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R32G32B32A32_FLOAT);

		myGBufferData.myRoughnessTexture = new Texture();
		myGBufferData.myRoughnessTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R32G32B32A32_FLOAT);
	}
}