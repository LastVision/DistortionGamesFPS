#include "stdafx.h"

#include "Camera.h"
#include <d3dx11effect.h>
#include "DeferredRenderer.h"
#include "IndexBufferWrapper.h"
#include "Texture.h"
#include "Scene.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SpotLightTextureProjection.h"
#include <XMLReader.h>

namespace Prism
{
	void RenderToScreenData::OnEffectLoad()
	{
		mySource = myEffect->GetEffect()->GetVariableByName("DiffuseTexture")->AsShaderResource();
	}

	void AmbientPass::OnEffectLoad()
	{
		myAlbedo = myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myNormal = myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myEmissive = myEffect->GetEffect()->GetVariableByName("EmissiveTexture")->AsShaderResource();
		myDepth = myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();
		myCubemap = myEffect->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();
		mySSAORandomTextureVariable = myEffect->GetEffect()->GetVariableByName("RandomTexture")->AsShaderResource();
		myAmbientMultiplier = myEffect->GetEffect()->GetVariableByName("AmbientMultiplier")->AsScalar();

		mycAr = myEffect->GetEffect()->GetVariableByName("cAr")->AsShaderResource();
		mycAg = myEffect->GetEffect()->GetVariableByName("cAg")->AsShaderResource();
		mycAb = myEffect->GetEffect()->GetVariableByName("cAb")->AsShaderResource();
		mycBr = myEffect->GetEffect()->GetVariableByName("cBr")->AsShaderResource();
		mycBg = myEffect->GetEffect()->GetVariableByName("cBg")->AsShaderResource();
		mycBb = myEffect->GetEffect()->GetVariableByName("cBb")->AsShaderResource();
		mycC = myEffect->GetEffect()->GetVariableByName("cC")->AsShaderResource();

		mySHGridSizeVariable = myEffect->GetEffect()->GetVariableByName("SHGridSize")->AsVector();
		mySHGridOffsetVariable = myEffect->GetEffect()->GetVariableByName("SHGridOffset")->AsVector();
		myCameraPositionVariable = myEffect->GetEffect()->GetVariableByName("cameraPosition")->AsVector();

		myInvertedProjection = myEffect->GetEffect()->GetVariableByName("InvertedProjection")->AsMatrix();
		myNotInvertedView = myEffect->GetEffect()->GetVariableByName("NotInvertedView")->AsMatrix();


#ifdef USE_LIGHT
		myAmbientMultiplier->SetFloat(0.05f);
#else
		myAmbientMultiplier->SetFloat(1.f);
#endif
	}

	void PointLightPass::OnEffectLoad()
	{
		myAlbedo = myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myNormal = myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myEmissive = myEffect->GetEffect()->GetVariableByName("EmissiveTexture")->AsShaderResource();
		myDepth = myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();
		myCubemap = myEffect->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();

		myPointLightVariable = myEffect->GetEffect()->GetVariableByName("PointLights");

		myInvertedProjection = myEffect->GetEffect()->GetVariableByName("InvertedProjection")->AsMatrix();
		myNotInvertedView = myEffect->GetEffect()->GetVariableByName("NotInvertedView")->AsMatrix();

		myCameraPositionVariable = myEffect->GetEffect()->GetVariableByName("cameraPosition")->AsVector();
	}

	void SpotLightPass::OnEffectLoad()
	{
		myAlbedo = myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myNormal = myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myEmissive = myEffect->GetEffect()->GetVariableByName("EmissiveTexture")->AsShaderResource();
		myDepth = myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();
		myCubemap = myEffect->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();

		mySpotLightVariable = myEffect->GetEffect()->GetVariableByName("SpotLights");

		myInvertedProjection = myEffect->GetEffect()->GetVariableByName("InvertedProjection")->AsMatrix();
		myNotInvertedView = myEffect->GetEffect()->GetVariableByName("NotInvertedView")->AsMatrix();

		myCameraPositionVariable = myEffect->GetEffect()->GetVariableByName("cameraPosition")->AsVector();
	}

	DeferredRenderer::DeferredRenderer()
	{
		CU::Vector2<float> windowSize = Engine::GetInstance()->GetWindowSize();

		myViewPort = new D3D11_VIEWPORT();
		ZeroMemory(myViewPort, sizeof(D3D11_VIEWPORT));

		myViewPort->TopLeftX = 0;
		myViewPort->TopLeftY = 0;
		myViewPort->Width = windowSize.x;
		myViewPort->Height = windowSize.y;
		myViewPort->MinDepth = 0.f;
		myViewPort->MaxDepth = 1.f;

		myDepthStencilTexture = new Texture();
		myDepthStencilTexture->InitAsDepthBuffer(windowSize.x, windowSize.y);
		myArmDepthStencilTexture = new Texture();
		myArmDepthStencilTexture->InitAsDepthBuffer(windowSize.x, windowSize.y);

		myClearColor[0] = 0.f;
		myClearColor[1] = 0.f;
		myClearColor[2] = 0.f;
		myClearColor[3] = 0.f;

		myRenderToScreenData.myEffect = 
			EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_render_to_texture.fx");
		myRenderToScreenData.OnEffectLoad();
		myRenderToScreenData.myEffect->AddListener(&myRenderToScreenData);

		SetupAmbientData();
		SetupLightData();
		SetupGBufferData();

		InitFullscreenQuad();

		//myCubemap = TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/CubeMap/T_cubemap_level01.dds");
		myCubeMapGenerator = new CubeMapGenerator();

		myFinishedTexture = new Texture();
		myFinishedTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R8G8B8A8_UNORM);

		XMLReader reader;
		reader.OpenDocument("Data/Setting/SET_rendering.xml");
		tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");
		reader.ForceReadAttribute(reader.ForceFindFirstChild(root, "shsize"), "value", GC::SHNodeSize);
		reader.CloseDocument();
	}

	DeferredRenderer::~DeferredRenderer()
	{
		SAFE_DELETE(myFinishedTexture);
		SAFE_DELETE(myViewPort);
		SAFE_DELETE(myDepthStencilTexture);
		SAFE_DELETE(myCubeMapGenerator);
		SAFE_DELETE(myGBufferData.myAlbedoTexture);
		SAFE_DELETE(myGBufferData.myNormalTexture);
		SAFE_DELETE(myGBufferData.myEmissiveTexture);
		SAFE_DELETE(myGBufferData.myDepthTexture);
	}

	void DeferredRenderer::Render(Scene* aScene, ID3D11RenderTargetView* aRenderTarget, ID3D11DepthStencilView* aDepthStencil)
	{
		myOcculusRenderTarget = aRenderTarget;
		myOcculusDepthStencil = aDepthStencil;
		//Engine::GetInstance()->RestoreViewPort();

		ClearGBuffer();
		Engine::GetInstance()->GetContex()->ClearDepthStencilView(myDepthStencilTexture->GetDepthStencilView()
			, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		Engine::GetInstance()->GetContex()->ClearDepthStencilView(myArmDepthStencilTexture->GetDepthStencilView()
			, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Engine::GetInstance()->GetContex()->RSSetViewports(1, myViewPort);

		SetGBufferAsTarget(myDepthStencilTexture);
		aScene->Render();
		aScene->RenderArmAndWeaponOnlyDepth();

		SetGBufferAsTarget(myArmDepthStencilTexture);
		aScene->RenderArmAndWeapon();

		ActivateBuffers();

		RenderDeferred(aScene);

		//Engine::GetInstance()->GetContex()->CopyResource(myTexture, aTexture);
	}

	void DeferredRenderer::RenderCubeMap(Scene* aScene, ID3D11RenderTargetView* aRenderTarget, ID3D11DepthStencilView* aDepth,
		D3D11_VIEWPORT* aViewPort, bool aUseAmbientPass)
	{
		Engine::GetInstance()->GetContex()->RSSetViewports(1, aViewPort);
		ClearGBuffer();
		Engine::GetInstance()->GetContex()->ClearDepthStencilView(myDepthStencilTexture->GetDepthStencilView()
			, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		Engine::GetInstance()->GetContex()->ClearDepthStencilView(myArmDepthStencilTexture->GetDepthStencilView()
			, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		SetGBufferAsTarget(myDepthStencilTexture);

		aScene->RenderWithoutRoomManager();

		ActivateBuffers();

		RenderCubemapDeferred(aScene, aRenderTarget, aDepth, aUseAmbientPass);
	}

	void DeferredRenderer::OnResize(float aWidth, float aHeight)
	{
		myGBufferData.myAlbedoTexture->Resize(aWidth, aHeight);
		myGBufferData.myNormalTexture->Resize(aWidth, aHeight);
		myGBufferData.myEmissiveTexture->Resize(aWidth, aHeight);
		myGBufferData.myDepthTexture->Resize(aWidth, aHeight);
		myDepthStencilTexture->Resize(aWidth, aHeight);
		myArmDepthStencilTexture->Resize(aWidth, aHeight);
		myFinishedTexture->Resize(aWidth, aHeight);
	}

	void DeferredRenderer::GenerateCubemap(Scene* aScene, const std::string& aName)
	{
		if (GC::GenerateCubeMap == true)
		{
			myCubeMapGenerator->GenerateCubeMap(this, aScene, { 0.f, 1.75f, 0.f }, { 1024.f, 1024.f }, true, "LightData/" + aName + "_cubemap.dds");
		}
	}

	void DeferredRenderer::GenerateSHData(Scene* aScene
		, const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint, const std::string& aName)
	{
		if (GC::GenerateLightData == true)
		{
			myAmbientPass.mySHGridSize.x = float(CU::Math::ClosestPowerOfTwo(abs(int(aMaxPoint.x - aMinPoint.x))));
			myAmbientPass.mySHGridSize.y = float(CU::Math::ClosestPowerOfTwo(abs(int(aMaxPoint.y - aMinPoint.y))));
			myAmbientPass.mySHGridSize.z = float(CU::Math::ClosestPowerOfTwo(abs(int(aMaxPoint.z - aMinPoint.z))));
			myAmbientPass.mySHGridOffset = aMinPoint;

			ModelLoader::GetInstance()->WaitUntilFinished();
			myCubeMapGenerator->GenerateSHTextures(this, aScene, mySHTextures, myAmbientPass.mySHGridSize
				, myAmbientPass.mySHGridOffset, GC::SHNodeSize, aName);

			myAmbientPass.mySHGridOffset *= -1.f;
		}
	}

	void DeferredRenderer::LoadSHData(const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint, const std::string& aName)
	{
		ModelLoader::GetInstance()->Pause();
		myAmbientPass.mySHGridSize.x = float(CU::Math::ClosestPowerOfTwo(abs(int(aMaxPoint.x - aMinPoint.x))));
		myAmbientPass.mySHGridSize.y = float(CU::Math::ClosestPowerOfTwo(abs(int(aMaxPoint.y - aMinPoint.y))));
		myAmbientPass.mySHGridSize.z = float(CU::Math::ClosestPowerOfTwo(abs(int(aMaxPoint.z - aMinPoint.z))));
		myAmbientPass.mySHGridOffset = aMinPoint;
		myAmbientPass.mySHGridOffset *= -1.f;

		mySHTextures.cAr = new Texture();
		mySHTextures.cAg = new Texture();
		mySHTextures.cAb = new Texture();
		mySHTextures.cBr = new Texture();
		mySHTextures.cBg = new Texture();
		mySHTextures.cBb = new Texture();
		mySHTextures.cC = new Texture();

		mySHTextures.cAr->LoadTexture("LightData/" + aName + "_cAr.dds");
		mySHTextures.cAg->LoadTexture("LightData/" + aName + "_cAg.dds");
		mySHTextures.cAb->LoadTexture("LightData/" + aName + "_cAb.dds");
		mySHTextures.cBr->LoadTexture("LightData/" + aName + "_cBr.dds");
		mySHTextures.cBg->LoadTexture("LightData/" + aName + "_cBg.dds");
		mySHTextures.cBb->LoadTexture("LightData/" + aName + "_cBb.dds");
		mySHTextures.cC->LoadTexture("LightData/" + aName + "_cC.dds");

		ModelLoader::GetInstance()->UnPause();
	}

	void DeferredRenderer::SetCubeMap(const std::string& aFilePath)
	{
		ModelLoader::GetInstance()->Pause();
		myCubemap = TextureContainer::GetInstance()->GetTexture("LightData/" + aFilePath + "_cubemap.dds");
		ModelLoader::GetInstance()->UnPause();
	}

	Prism::Texture* DeferredRenderer::GetFinishedTexture()
	{
		return myFinishedTexture;
	}

	Prism::Texture* DeferredRenderer::GetEmissiveTexture()
	{
		return myGBufferData.myEmissiveTexture;
	}

	Prism::Texture* DeferredRenderer::GetDepthStencilTexture()
	{
		return myDepthStencilTexture;
	}

	Prism::Texture* DeferredRenderer::GetArmDepthStencilTexture()
	{
		return myArmDepthStencilTexture;
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

	void DeferredRenderer::Render(Effect* aEffect, const std::string& aTechnique)
	{
		D3DX11_TECHNIQUE_DESC techDesc;
		ID3DX11EffectTechnique* technique = aEffect->GetTechnique(aTechnique);

		technique->GetDesc(&techDesc);
		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			technique->GetPassByIndex(i)->Apply(0, Engine::GetInstance()->GetContex());
			Engine::GetInstance()->GetContex()->DrawIndexed(6, 0, 0);
		}
	}

	void DeferredRenderer::RenderDeferred(Scene* aScene)
	{
		//Engine::GetInstance()->RestoreViewPort();

		RenderAmbientPass(aScene);

		//ID3D11RenderTargetView* renderTarget = Engine::GetInstance()->GetBackbuffer();
		//ID3D11RenderTargetView* renderTarget = myFinishedTexture->GetRenderTargetView();
		//Engine::GetInstance()->GetContex()->OMSetRenderTargets(1, &renderTarget, myDepthStencilTexture->GetDepthStencilView());

		ID3D11RenderTargetView* renderTarget = myOcculusRenderTarget;
		Engine::GetInstance()->GetContex()->OMSetRenderTargets(1, &renderTarget, myOcculusDepthStencil);

#ifdef USE_LIGHT
		aScene->UpdateLights();
		RenderPointLights(aScene);
		RenderSpotLights(aScene);
		RenderSpotLightsTextureProjection(aScene);
#endif
	}

	void DeferredRenderer::RenderCubemapDeferred(Scene* aScene, ID3D11RenderTargetView* aTarget
		, ID3D11DepthStencilView* aDepth, bool aUseAmbientPass)
	{
		if (aUseAmbientPass == true)
		{
			RenderAmbientPass(aScene);
		}

		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();
		context->OMSetRenderTargets(1, &aTarget, aDepth);

#ifdef USE_LIGHT
		aScene->UpdateLights();
		RenderPointLights(aScene, false);
		//RenderSpotLights(aScene, false);
#endif
	}

	void DeferredRenderer::RenderPointLights(Scene* aScene, bool aUseRoomManager)
	{
		const Camera& camera = *aScene->GetCamera();

		//Engine::GetInstance()->RestoreViewPort();
		SetPointLightData(camera);

		const CU::GrowingArray<PointLight*>& lights = aScene->GetPointLights(aUseRoomManager);

		Engine::GetInstance()->SetRasterizeState(eRasterizer::NO_CULLING);
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::READ_NO_WRITE);
		for (int i = 0; i < lights.Size(); ++i)
		{
			myPointLightPass.myPointLightVariable->SetRawValue(&lights[i]->GetLightData(), 0, sizeof(PointLightData));
			lights[i]->Render(camera);
		}
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);

		RemovePointLightData();
	}

	void DeferredRenderer::RenderSpotLights(Scene* aScene, bool aUseRoomManager)
	{
		const Camera& camera = *aScene->GetCamera();

		Engine::GetInstance()->RestoreViewPort();
		SetSpotLightData(camera);


		const CU::GrowingArray<SpotLight*>& lights = aScene->GetSpotLights(aUseRoomManager);

		Engine::GetInstance()->SetRasterizeState(eRasterizer::NO_CULLING);
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::READ_NO_WRITE);
		for (int i = 0; i < lights.Size(); ++i)
		{
			mySpotLightPass.mySpotLightVariable->SetRawValue(&lights[i]->GetLightData(), 0, sizeof(SpotLightData));
			lights[i]->Render(camera);
		}
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);

		RemoveSpotLightData();
	}

	void DeferredRenderer::RenderSpotLightsTextureProjection(Scene* aScene, bool aUseRoomManager)
	{
		const Camera& camera = *aScene->GetCamera();

		Engine::GetInstance()->RestoreViewPort();
		SetSpotLightTextureProjectionData(camera);

		const CU::GrowingArray<SpotLightTextureProjection*>& lights = aScene->GetSpotLightsTextureProjection(aUseRoomManager);

		Engine::GetInstance()->SetRasterizeState(eRasterizer::NO_CULLING);
		//Engine::GetInstance()->SetDepthBufferState(eDepthStencil::READ_NO_WRITE);
		for (int i = 0; i < lights.Size(); ++i)
		{
			mySpotLightTextureProjectionPass.mySpotLightVariable->SetRawValue(&lights[i]->GetLightData(), 0, sizeof(SpotLightData));
			
			lights[i]->Render(camera);
		}
		//Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);

		RemoveSpotLightTextureProjectionData();
	}

	void DeferredRenderer::RenderAmbientPass(Scene* aScene)
	{
		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();

		//ID3D11RenderTargetView* backbuffer = Engine::GetInstance()->GetBackbuffer();
		//ID3D11RenderTargetView* backbuffer = myFinishedTexture->GetRenderTargetView();
		ID3D11RenderTargetView* backbuffer = myOcculusRenderTarget;
		context->ClearRenderTargetView(backbuffer, myClearColor);
		context->ClearDepthStencilView(Engine::GetInstance()->GetDepthView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		/*context->OMSetRenderTargets(1, &backbuffer
			, Engine::GetInstance()->GetDepthView());*/
		context->OMSetRenderTargets(1, &backbuffer
			, myOcculusDepthStencil);

		SetAmbientData(false);
		myAmbientPass.myEffect->SetCameraPosition(aScene->GetCamera()->GetOrientation().GetPos());
		myAmbientPass.myInvertedProjection->SetMatrix(&CU::InverseReal(aScene->GetCamera()->GetProjection()).myMatrix[0]);
		myAmbientPass.myNotInvertedView->SetMatrix(&CU::InverseReal(aScene->GetCamera()->GetOrientation()).myMatrix[0]);

		myAmbientPass.mySHGridSizeVariable->SetFloatVector(&myAmbientPass.mySHGridSize.x);
		myAmbientPass.mySHGridOffsetVariable->SetFloatVector(&myAmbientPass.mySHGridOffset.x);

		CU::Vector3<float> cameraPos = aScene->GetCamera()->GetOrientation().GetPos();
		myAmbientPass.myCameraPositionVariable->SetFloatVector(&cameraPos.x);
		
		if (GC::EnableCheapAmbient == true)
		{
			Render(myAmbientPass.myEffect, "Render_Cheap");
		}
		else
		{
			Render(myAmbientPass.myEffect, "Render_Cheap");

			//Render(myAmbientPass.myEffect);
		}

		SetAmbientData(true);
	}

	void DeferredRenderer::SetAmbientData(bool aClearTextures)
	{
		if (aClearTextures == false)
		{
			myAmbientPass.myAlbedo->SetResource(myGBufferData.myAlbedoTexture->GetShaderView());
			myAmbientPass.myNormal->SetResource(myGBufferData.myNormalTexture->GetShaderView());
			myAmbientPass.myEmissive->SetResource(myGBufferData.myEmissiveTexture->GetShaderView());
			myAmbientPass.myDepth->SetResource(myGBufferData.myDepthTexture->GetShaderView());
			myAmbientPass.myCubemap->SetResource(myCubemap->GetShaderView());
			myAmbientPass.mySSAORandomTextureVariable->SetResource(myAmbientPass.mySSAORandomTexture->GetShaderView());

			//myAmbientPass.mycAr->SetResource(mySHTextures.cAr->GetShaderView());
			//myAmbientPass.mycAg->SetResource(mySHTextures.cAg->GetShaderView());
			//myAmbientPass.mycAb->SetResource(mySHTextures.cAb->GetShaderView());
			//myAmbientPass.mycBr->SetResource(mySHTextures.cBr->GetShaderView());
			//myAmbientPass.mycBg->SetResource(mySHTextures.cBg->GetShaderView());
			//myAmbientPass.mycBb->SetResource(mySHTextures.cBb->GetShaderView());
			//myAmbientPass.mycC->SetResource(mySHTextures.cC->GetShaderView());
		}
		else
		{
			myAmbientPass.myAlbedo->SetResource(nullptr);
			myAmbientPass.myNormal->SetResource(nullptr);
			myAmbientPass.myEmissive->SetResource(nullptr);
			myAmbientPass.myDepth->SetResource(nullptr);
			myAmbientPass.myCubemap->SetResource(nullptr);
			myAmbientPass.mySSAORandomTextureVariable->SetResource(nullptr);

			myAmbientPass.mycAr->SetResource(nullptr);
			myAmbientPass.mycAg->SetResource(nullptr);
			myAmbientPass.mycAb->SetResource(nullptr);
			myAmbientPass.mycBr->SetResource(nullptr);
			myAmbientPass.mycBg->SetResource(nullptr);
			myAmbientPass.mycBb->SetResource(nullptr);
			myAmbientPass.mycC->SetResource(nullptr);
		}
	}

	void DeferredRenderer::SetupAmbientData()
	{
		myAmbientPass.myEffect = EffectContainer::GetInstance()->GetEffect(
			"Data/Resource/Shader/S_effect_deferred_ambient.fx");

		myAmbientPass.OnEffectLoad();
		myAmbientPass.myEffect->AddListener(&myAmbientPass);

		myAmbientPass.mySSAORandomTexture = TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/T_random_texture.dds");
	}

	void DeferredRenderer::SetupLightData()
	{
		myPointLightPass.myEffect = EffectContainer::GetInstance()->GetEffect(
			"Data/Resource/Shader/S_effect_deferred_light_mesh_point.fx");

		myPointLightPass.OnEffectLoad();
		myPointLightPass.myEffect->AddListener(&myPointLightPass);


		mySpotLightPass.myEffect = EffectContainer::GetInstance()->GetEffect(
			"Data/Resource/Shader/S_effect_deferred_light_mesh_spot.fx");

		mySpotLightPass.OnEffectLoad();
		mySpotLightPass.myEffect->AddListener(&mySpotLightPass);


		mySpotLightTextureProjectionPass.myEffect = EffectContainer::GetInstance()->GetEffect(
			"Data/Resource/Shader/S_effect_deferred_light_mesh_spot_textureprojection.fx");

		mySpotLightTextureProjectionPass.OnEffectLoad();
		mySpotLightTextureProjectionPass.myEffect->AddListener(&mySpotLightTextureProjectionPass);
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

		myGBufferData.myEmissiveTexture = new Texture();
		myGBufferData.myEmissiveTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R8G8B8A8_UNORM);

		myGBufferData.myDepthTexture = new Texture();
		myGBufferData.myDepthTexture->Init(windowSize.x, windowSize.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R32_FLOAT);

	}

	void DeferredRenderer::ClearGBuffer()
	{
		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();

		context->ClearRenderTargetView(myGBufferData.myAlbedoTexture->GetRenderTargetView(), myClearColor);
		context->ClearRenderTargetView(myGBufferData.myNormalTexture->GetRenderTargetView(), myClearColor);
		context->ClearRenderTargetView(myGBufferData.myEmissiveTexture->GetRenderTargetView(), myClearColor);
		context->ClearRenderTargetView(myGBufferData.myDepthTexture->GetRenderTargetView(), myClearColor);
	}

	void DeferredRenderer::SetGBufferAsTarget(Texture* aDepthTexture)
	{
		ID3D11RenderTargetView* targets[4];
		targets[0] = myGBufferData.myAlbedoTexture->GetRenderTargetView();
		targets[1] = myGBufferData.myNormalTexture->GetRenderTargetView();
		targets[2] = myGBufferData.myEmissiveTexture->GetRenderTargetView();
		targets[3] = myGBufferData.myDepthTexture->GetRenderTargetView();

		Engine::GetInstance()->GetContex()->OMSetRenderTargets(4, targets
			, aDepthTexture->GetDepthStencilView());
	}

	void DeferredRenderer::SetPointLightData(const Camera& aCamera)
	{
		myPointLightPass.myAlbedo->SetResource(myGBufferData.myAlbedoTexture->GetShaderView());
		myPointLightPass.myNormal->SetResource(myGBufferData.myNormalTexture->GetShaderView());
		//myPointLightPass.myEmissive->SetResource(myGBufferData.myEmissiveTexture->GetShaderView());
		myPointLightPass.myDepth->SetResource(myGBufferData.myDepthTexture->GetShaderView());
		myPointLightPass.myCubemap->SetResource(myCubemap->GetShaderView());
		myPointLightPass.myInvertedProjection->SetMatrix(&CU::InverseReal(aCamera.GetProjection()).myMatrix[0]);
		myPointLightPass.myNotInvertedView->SetMatrix(&CU::InverseReal(aCamera.GetOrientation()).myMatrix[0]);

		CU::Vector3<float> cameraPos = aCamera.GetOrientation().GetPos();
		myPointLightPass.myCameraPositionVariable->SetFloatVector(&cameraPos.x);
	}

	void DeferredRenderer::RemovePointLightData()
	{
		myPointLightPass.myAlbedo->SetResource(nullptr);
		myPointLightPass.myNormal->SetResource(nullptr);
		myPointLightPass.myEmissive->SetResource(nullptr);
		myPointLightPass.myDepth->SetResource(nullptr);
	}

	void DeferredRenderer::SetSpotLightData(const Camera& aCamera)
	{
		mySpotLightPass.myAlbedo->SetResource(myGBufferData.myAlbedoTexture->GetShaderView());
		mySpotLightPass.myNormal->SetResource(myGBufferData.myNormalTexture->GetShaderView());
		//mySpotLightPass.myEmissive->SetResource(myGBufferData.myEmissiveTexture->GetShaderView());
		mySpotLightPass.myDepth->SetResource(myGBufferData.myDepthTexture->GetShaderView());
		mySpotLightPass.myCubemap->SetResource(myCubemap->GetShaderView());
		mySpotLightPass.myInvertedProjection->SetMatrix(&CU::InverseReal(aCamera.GetProjection()).myMatrix[0]);
		mySpotLightPass.myNotInvertedView->SetMatrix(&aCamera.GetOrientation().myMatrix[0]);

		CU::Vector3<float> cameraPos = aCamera.GetOrientation().GetPos();
		mySpotLightPass.myCameraPositionVariable->SetFloatVector(&cameraPos.x);
	}

	void DeferredRenderer::RemoveSpotLightData()
	{
		mySpotLightPass.myAlbedo->SetResource(nullptr);
		mySpotLightPass.myNormal->SetResource(nullptr);
		mySpotLightPass.myEmissive->SetResource(nullptr);
		mySpotLightPass.myDepth->SetResource(nullptr);
	}

	void DeferredRenderer::SetSpotLightTextureProjectionData(const Camera& aCamera)
	{
		mySpotLightTextureProjectionPass.myAlbedo->SetResource(myGBufferData.myAlbedoTexture->GetShaderView());
		mySpotLightTextureProjectionPass.myNormal->SetResource(myGBufferData.myNormalTexture->GetShaderView());
		//mySpotLightTextureProjectionPass.myEmissive->SetResource(myGBufferData.myEmissiveTexture->GetShaderView());
		mySpotLightTextureProjectionPass.myDepth->SetResource(myGBufferData.myDepthTexture->GetShaderView());
		mySpotLightTextureProjectionPass.myCubemap->SetResource(myCubemap->GetShaderView());
		mySpotLightTextureProjectionPass.myInvertedProjection->SetMatrix(&CU::InverseReal(aCamera.GetProjection()).myMatrix[0]);
		mySpotLightTextureProjectionPass.myNotInvertedView->SetMatrix(&aCamera.GetOrientation().myMatrix[0]);

		CU::Vector3<float> cameraPos = aCamera.GetOrientation().GetPos();
		mySpotLightTextureProjectionPass.myCameraPositionVariable->SetFloatVector(&cameraPos.x);
	}

	void DeferredRenderer::RemoveSpotLightTextureProjectionData()
	{
		mySpotLightTextureProjectionPass.myAlbedo->SetResource(nullptr);
		mySpotLightTextureProjectionPass.myNormal->SetResource(nullptr);
		mySpotLightTextureProjectionPass.myEmissive->SetResource(nullptr);
		mySpotLightTextureProjectionPass.myDepth->SetResource(nullptr);
	}
}