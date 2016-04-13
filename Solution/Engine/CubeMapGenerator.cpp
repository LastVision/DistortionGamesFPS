#include "stdafx.h"

#include "Camera.h"
#include "CubeMapGenerator.h"
#include "DeferredRenderer.h"
#include "Scene.h"
#include <MathHelper.h>
#include <D3DX11tex.h>

#define _USE_MATH_DEFINES
#include <math.h>

namespace Prism
{
	SHTextures::~SHTextures()
	{
		SAFE_DELETE(cAr);
		SAFE_DELETE(cAg);
		SAFE_DELETE(cAb);
		SAFE_DELETE(cBr);
		SAFE_DELETE(cBg);
		SAFE_DELETE(cBb);
		SAFE_DELETE(cC);
	}



	CubeMapGenerator::CubeMapGenerator()
	{
		myCamera = new Camera(myCurrentCameraOrientation, 512.f, 512.f);

		myCameraOrientations[0] = CU::Matrix44<float>::CreateRotateAroundY(static_cast<float>(M_PI_2));
		myCameraOrientations[1] = CU::Matrix44<float>::CreateRotateAroundY(static_cast<float>(-M_PI_2));
		myCameraOrientations[2] = CU::Matrix44<float>::CreateRotateAroundX(static_cast<float>(M_PI_2));
		myCameraOrientations[3] = CU::Matrix44<float>::CreateRotateAroundX(static_cast<float>(-M_PI_2));
		myCameraOrientations[4] = CU::Matrix44<float>();
		myCameraOrientations[5] = CU::Matrix44<float>::CreateRotateAroundY(static_cast<float>(M_PI));
	}


	CubeMapGenerator::~CubeMapGenerator()
	{
		SAFE_DELETE(myCamera);
	}

	void CubeMapGenerator::GenerateSHTextures(DeferredRenderer* aRenderer, Scene* aScene, SHTextures& someTextures
		, const CU::Vector3<float>& aGridSize, const CU::Vector3<float>& aOffset, float aNodeSize
		, const std::string& aName)
	{
		mySH_GRID_X = int(ceilf(aGridSize.x / aNodeSize));
		mySH_GRID_Y = int(ceilf(aGridSize.y / aNodeSize));
		mySH_GRID_Z = int(ceilf(aGridSize.z / aNodeSize));


		
		SHGridNode*** gridNodes = new SHGridNode**[mySH_GRID_X];
		for (int x = 0; x < mySH_GRID_X; ++x)
		{
			gridNodes[x] = new SHGridNode*[mySH_GRID_Y];
			for (int y = 0; y < mySH_GRID_Y; ++y)
			{
				gridNodes[x][y] = new SHGridNode[mySH_GRID_Z];
			}
		}

		CU::Vector3<float> offset(aOffset);
		Texture* cubeMap = nullptr;

		int totalNodeCount = mySH_GRID_X * mySH_GRID_Y * mySH_GRID_Z;


		DL_PRINT_VA("Generating %d SHNodes.", totalNodeCount);

		for (int z = 0; z < mySH_GRID_Z; ++z)
		{
			for (int y = 0; y < mySH_GRID_Y; ++y)
			{
				for (int x = 0; x < mySH_GRID_X; ++x)
				{
					std::stringstream ss;
					ss << "Data/SHNodes/Node_" << x << "_" << y << "_" << z << "_new.dds";
					CU::Vector3<float> pos = CU::Vector3<float>(float(x), float(y), float(z));
					pos *= aNodeSize;
					pos += aOffset;

					//cubeMap = GenerateCubeMap(aRenderer, aScene, pos, { 32.f, 32.f }, ss.str());
					cubeMap = GenerateCubeMap(aRenderer, aScene, pos, { 32.f, 32.f }, false, "");
					gridNodes[x][y][z] = GetSHGridNode(GenerateSHNode(cubeMap, pos));

					SAFE_DELETE(cubeMap);
					Engine::GetInstance()->GetContex()->Flush();
				}
			}

			int zPrim = z * mySH_GRID_X * mySH_GRID_Y;
			int finishedCount = zPrim;

			DL_PRINT_VA("%f %% of SHNodes Generated.", float(finishedCount) / float(totalNodeCount) * 100.f);
		}

		DL_PRINT("Generation DONE");


		CU::Vector4<float>* cAr = new CU::Vector4<float>[totalNodeCount];
		CU::Vector4<float>* cAg = new CU::Vector4<float>[totalNodeCount];
		CU::Vector4<float>* cAb = new CU::Vector4<float>[totalNodeCount];
		CU::Vector4<float>* cBr = new CU::Vector4<float>[totalNodeCount];
		CU::Vector4<float>* cBg = new CU::Vector4<float>[totalNodeCount];
		CU::Vector4<float>* cBb = new CU::Vector4<float>[totalNodeCount];
		CU::Vector4<float>* cC = new CU::Vector4<float>[totalNodeCount];

		for (int z = 0; z < mySH_GRID_Z; ++z)
		{
			for (int y = 0; y < mySH_GRID_Y; ++y)
			{
				for (int x = 0; x < mySH_GRID_X; ++x)
				{
					int xPrim = x;
					int yPrim = y * mySH_GRID_X;
					int zPrim = z * mySH_GRID_X * mySH_GRID_Y;
					int index = xPrim + yPrim + zPrim;

					cAr[index] = gridNodes[x][y][z].cAr;
					cAg[index] = gridNodes[x][y][z].cAg;
					cAb[index] = gridNodes[x][y][z].cAb;
					cBr[index] = gridNodes[x][y][z].cBr;
					cBg[index] = gridNodes[x][y][z].cBg;
					cBb[index] = gridNodes[x][y][z].cBb;
					cC[index] = gridNodes[x][y][z].cC;
				}
			}
		}

		someTextures.cAr = new Texture();
		someTextures.cAg = new Texture();
		someTextures.cAb = new Texture();
		someTextures.cBr = new Texture();
		someTextures.cBg = new Texture();
		someTextures.cBb = new Texture();
		someTextures.cC = new Texture();

		someTextures.cAr->Create3DTexture(&cAr[0].x, mySH_GRID_X, mySH_GRID_Y, mySH_GRID_Z);
		someTextures.cAg->Create3DTexture(&cAg[0].x, mySH_GRID_X, mySH_GRID_Y, mySH_GRID_Z);
		someTextures.cAb->Create3DTexture(&cAb[0].x, mySH_GRID_X, mySH_GRID_Y, mySH_GRID_Z);
		someTextures.cBr->Create3DTexture(&cBr[0].x, mySH_GRID_X, mySH_GRID_Y, mySH_GRID_Z);
		someTextures.cBg->Create3DTexture(&cBg[0].x, mySH_GRID_X, mySH_GRID_Y, mySH_GRID_Z);
		someTextures.cBb->Create3DTexture(&cBb[0].x, mySH_GRID_X, mySH_GRID_Y, mySH_GRID_Z);
		someTextures.cC->Create3DTexture(&cC[0].x, mySH_GRID_X, mySH_GRID_Y, mySH_GRID_Z);

		someTextures.cAr->Save3DToFile("LightData/" + aName + "_cAr_new.dds");
		someTextures.cAg->Save3DToFile("LightData/" + aName + "_cAg_new.dds");
		someTextures.cAb->Save3DToFile("LightData/" + aName + "_cAb_new.dds");
		someTextures.cBr->Save3DToFile("LightData/" + aName + "_cBr_new.dds");
		someTextures.cBg->Save3DToFile("LightData/" + aName + "_cBg_new.dds");
		someTextures.cBb->Save3DToFile("LightData/" + aName + "_cBb_new.dds");
		someTextures.cC->Save3DToFile( "LightData/" + aName + "_cC_new.dds" );

		SAFE_ARRAY_DELETE(cAr);
		SAFE_ARRAY_DELETE(cAg);
		SAFE_ARRAY_DELETE(cAb);
		SAFE_ARRAY_DELETE(cBr);
		SAFE_ARRAY_DELETE(cBg);
		SAFE_ARRAY_DELETE(cBb);
		SAFE_ARRAY_DELETE(cC);

		for (int x = 0; x < mySH_GRID_X; ++x)
		{
			for (int y = 0; y < mySH_GRID_Y; ++y)
			{
				SAFE_ARRAY_DELETE(gridNodes[x][y]);
			}
			SAFE_ARRAY_DELETE(gridNodes[x]);
		}
		SAFE_ARRAY_DELETE(gridNodes);
	}

	void CubeMapGenerator::OnResize(float aWidth, float aHeight)
	{
		myCamera->OnResize(int(aWidth), int(aHeight));
	}

	Texture* CubeMapGenerator::GenerateCubeMap(DeferredRenderer* aRenderer, Scene* aScene, const CU::Vector3<float> aCameraPosition
		, const CU::Vector2<float>& aTextureSize, bool aUseAmbientPass, const std::string& aFileName)
	{
		
		const Camera* sceneCamera = aScene->GetCamera();
		Texture* cubemapTexture = new Texture();
		cubemapTexture->CreateCubemap(aTextureSize.x, aTextureSize.y);
		cubemapTexture->CreateDepthStencilView(aTextureSize.x, aTextureSize.y, 1);

		//cubemapTexture->CreateCubemap(512.f, 512.f);
		//cubemapTexture->CreateDepthStencilView(512.f, 512.f, 1);


		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();
		float clearcolor[4] = { 0.3f, 0.3f, 0.3f, 1 };

		context->RSSetViewports(1, cubemapTexture->GetViewPort());
		aScene->SetCamera(*myCamera);
		for (int i = 0; i < 6; ++i)
		{
			ID3D11RenderTargetView* target = cubemapTexture->GetCubemapRenderTarget(i);
			ID3D11DepthStencilView* depth = cubemapTexture->GetDepthStencilView();

			context->ClearRenderTargetView(target, clearcolor);
			context->ClearDepthStencilView(depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);


			myCameraOrientations[i].SetPos(aCameraPosition);
			myCurrentCameraOrientation = myCameraOrientations[i];
			myCamera->Update(1.f / 30.f);
			aRenderer->RenderCubeMap(aScene, target, depth, cubemapTexture->GetViewPort(), aUseAmbientPass);
		}


		Engine::GetInstance()->RestoreViewPort();
		ID3D11RenderTargetView* backbuffer = Engine::GetInstance()->GetBackbuffer();
		Engine::GetInstance()->GetContex()->ClearRenderTargetView(backbuffer, clearcolor);
		Engine::GetInstance()->GetContex()->OMSetRenderTargets(1, &backbuffer
			, Engine::GetInstance()->GetDepthView());

		aScene->SetCamera(*sceneCamera);
		if (aFileName != "")
		{
			cubemapTexture->SaveToFile(aFileName);
		}

		return cubemapTexture;
	}

	SHNode CubeMapGenerator::GenerateSHNode(Texture* aTexture, const CU::Vector3<float> aCameraPosition)
	{
		SHNode output;
		ID3D11Resource* textureResource;
		aTexture->GetShaderView()->GetResource(&textureResource);

		D3DX11SHProjectCubeMap(Engine::GetInstance()->GetContex(), 3, reinterpret_cast<ID3D11Texture2D*>(textureResource), output.myData[0], output.myData[1], output.myData[2]);

		output.position = CU::Vector4<float>(aCameraPosition.x, aCameraPosition.y, aCameraPosition.z, 1.f);
		return output;
	}

	Prism::SHGridNode CubeMapGenerator::GetSHGridNode(const SHNode& aNode)
	{
		SHGridNode gridNode;

		CU::Vector4<float> coeff[3];

		static const float s_fSqrtPI = ((float)sqrt(M_PI));
		const float fC0 = 1.0f / (2.0f*s_fSqrtPI);
		const float fC1 = (float)sqrt(3.0f) / (3.0f*s_fSqrtPI);
		const float fC2 = (float)sqrt(15.0f) / (8.0f*s_fSqrtPI);
		const float fC3 = (float)sqrt(5.0f) / (16.0f*s_fSqrtPI);
		const float fC4 = 0.5f*fC2;

		for (int i = 0; i < 3; ++i)
		{
			coeff[i].x = -fC1*aNode.myData[i][3];
			coeff[i].y = -fC1*aNode.myData[i][1];
			coeff[i].z = fC1*aNode.myData[i][2];
			coeff[i].w = fC0*aNode.myData[i][0] - fC3*aNode.myData[i][6];
		}

		gridNode.cAr = coeff[0];
		gridNode.cAg = coeff[1];
		gridNode.cAb = coeff[2];

		for (int i = 0; i < 3; ++i)
		{
			coeff[i].x = fC2*aNode.myData[i][4];
			coeff[i].y = -fC2*aNode.myData[i][5];
			coeff[i].z = 3.f*fC3*aNode.myData[i][6];
			coeff[i].w = -fC2*aNode.myData[i][7];
		}


		gridNode.cBr = coeff[0];
		gridNode.cBg = coeff[1];
		gridNode.cBb = coeff[2];

		coeff[0].x = fC4*aNode.myData[0][8];
		coeff[0].y = fC4*aNode.myData[1][8];
		coeff[0].z = fC4*aNode.myData[2][8];
		coeff[0].w = 1.f;

		gridNode.cC = coeff[0];

		return gridNode;
	}
}