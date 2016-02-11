#include "stdafx.h"

#include "HeightMap.h"
#include "HeightMapFactory.h"
#include "Terrain.h"
#include <CommonHelper.h>
#include <D3D11.h>
#include <d3dx11effect.h>
#include "Camera.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "Engine.h"
#include "Ice.h"
#include "IndexBufferWrapper.h"
#include <MathHelper.h>
#include "NavMesh.h"
#include "PathFinderFunnel.h"
#include "Surface.h"
#include "SplatMapContainer.h"
#include "TextureContainer.h"
#include "VertexBufferWrapper.h"
#include "VertexIndexWrapper.h"
#include "VertexDataWrapper.h"

#include <TimerManager.h>

namespace Prism
{
	Terrain::Terrain(const std::string& aHeightMapPath, const std::string& aTexturePath, const std::string& aNormalMapPath
			, const CU::Vector2<float>& aSize, float aHeight, const CU::Matrix44<float>& aOrientation
			, const std::string& aIceInfluence)
		: myHeightMap(HeightMapFactory::Create(aHeightMapPath.c_str()))
		, mySize(aSize)
		, myHeight(aHeight)
		, myOrientation(aOrientation)
		, myPathFinder(nullptr)
		, myNavMesh(nullptr)
		, myIceHeight(0.9f)
	{
		DL_ASSERT_EXP(mySize.x == mySize.y, "Can't create non-quad terrain.");

		myFileName = aTexturePath;
		myNormalMapFilePath = aNormalMapPath;
		myCellSize = mySize.x / myHeightMap->myWidth;

		CreateVertices();

		SetupDirectXData(aIceInfluence);
	}

	Terrain::Terrain(const std::string& aHeightMapPath, const CU::Vector2<float>& aSize, float aHeight)
		: mySize(aSize)
		, myHeight(aHeight)
		, myIceHeight(0.9f)
	{
		myHeightMap = HeightMapFactory::Create(aHeightMapPath.c_str());

		myCellSize = mySize.x / myHeightMap->myWidth;
		CreateVertices();
	}

	Terrain::Terrain(const std::string& aBinaryPath, const std::string& aTexturePath, const std::string& aNormalMapPath
		, const std::string& aIceInfluence)
		: myIceHeight(0.9f)
	{
		myFileName = aTexturePath;
		myNormalMapFilePath = aNormalMapPath;
		std::fstream file;
		file.open(aBinaryPath, std::ios::in | std::ios::binary);

		if (file.fail())
		{
			DL_ASSERT("Failed to open Terrain-binary-path, did you forget to run the tool?");
			return;
		}
		int terrainVersion;
		file.read((char*)&terrainVersion, sizeof(int));
		DL_ASSERT_EXP(terrainVersion == TERRAIN_VERSION, "Wrong Terrain Version, did you forget to run the tool?");

		myHeightMap = new HeightMap(file);


		file.read((char*)&mySize.x, sizeof(float) * 2);
		file.read((char*)&myHeight, sizeof(float));

		myCellSize = mySize.x / myHeightMap->myWidth;

		int vertexCount;
		file.read((char*)&vertexCount, sizeof(int));
		myVertices.Reserve(vertexCount);
		file.read((char*)&myVertices[0], sizeof(myVertices[0]) * vertexCount);


		int indexCount;
		file.read((char*)&indexCount, sizeof(int));
		myIndices.Reserve(indexCount);
		file.read((char*)&myIndices[0], sizeof(int) * indexCount);

		myNavMesh = new Navigation::NavMesh(file);


		SetupDirectXData(aIceInfluence);
	}

	Terrain::~Terrain()
	{
		SAFE_DELETE(myHeightMap);
		SAFE_DELETE(myPathFinder);
		SAFE_DELETE(myNavMesh);
		SAFE_DELETE(mySplatMapContainer);
		SAFE_DELETE(myIce);
		SAFE_DELETE(myIndexBaseData);
		SAFE_DELETE(myVertexBaseData);
	}

	void Terrain::SetupDirectXData(const std::string& aIceInfluence)
	{
		CU::TimerManager::GetInstance()->StartTimer("SetupDirectXData");


		myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_terrain.fx");
		myEffect->SetTexture(TextureContainer::GetInstance()->GetTexture(myNormalMapFilePath));
		//Texture * influence = Prism::TextureContainer::GetInstance()
		//->GetTexture("Data/Resource/Texture/Terrain/SplatMap/T_InfluenceToSplatMap.dds");
		//myEffect->SetTexture(influence);

		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};



		InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "Terrain::InputLayout");
		InitVertexBuffer(sizeof(VertexPosNormUVBiTang), D3D11_USAGE_IMMUTABLE, 0);

		InitIndexBuffer();
		InitSurface("AlbedoTexture", myFileName);

		mySplatMapContainer = new SplatMapContainer(myEffect);

		mySplatMapContainer->SetTextures();


		InitBlendState("Terrain::BlendState");

		ZeroMemory(myInitData, sizeof(*myInitData));

		myIce = new Ice(EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_ice.fx")
			, { 256.f, 256.f }, myIceHeight, aIceInfluence);
		myIce->SetTextures();


		SetupVertexBuffer(myVertices.Size(), sizeof(VertexPosNormUVBiTang), reinterpret_cast<char*>(&myVertices[0])
			, "Terrain::VertexBuffer");
		SetupIndexBuffer(myIndices.Size(), reinterpret_cast<char*>(&myIndices[0]), "Terrain::IndexBuffer");

		mySurfaces[0]->SetVertexCount(myVertices.Size());
		mySurfaces[0]->SetIndexCount(myIndices.Size());

		int elapsed = static_cast<int>(
			CU::TimerManager::GetInstance()->StopTimer("SetupDirectXData").GetMilliseconds());
		RESOURCE_LOG("SetupDirectXData took %d ms", elapsed);
	}

	void Terrain::Render(const Camera& aCamera, bool aRenderNavMeshLines, bool aIsDepthRender)
	{

		CU::Matrix44<float> world;
		//world.SetPos(CU::Vector3<float>(0, -110.f, 0));
		myEffect->SetWorldMatrix(world);
		myEffect->SetViewProjectionMatrix(aCamera.GetViewProjection());
		myEffect->SetCameraPosition(aCamera.GetOrientation().GetPos());
		BaseModel::Render(aIsDepthRender);
		myIce->Render(aCamera);
		myNavMesh->Render(aRenderNavMeshLines);
	}

	void Terrain::CalcEntityHeight(CU::Matrix44<float>& anOrientation) const
	{
		anOrientation.SetPos(GetHeight(anOrientation.GetPos(), 0));
	}


	CU::Vector3<float> Terrain::GetHeight(const CU::Vector2<float>& aPosition, float aHeightOffset) const
	{
		return GetHeight({ aPosition.x, 0, aPosition.y }, aHeightOffset);
	}

	CU::Vector3<float> Terrain::GetHeight(const CU::Vector3<float>& aPosition, float aHeightOffset) const
	{
		CU::Vector3<float> position00(aPosition);
		CU::Vector3<float> position10({ aPosition.x + myCellSize, aPosition.y, aPosition.z });
		CU::Vector3<float> position01({ aPosition.x, aPosition.y, aPosition.z + myCellSize });
		CU::Vector3<float> position11({ aPosition.x + myCellSize, aPosition.y, aPosition.z + myCellSize });

		GetPoint(position00);
		GetPoint(position10);
		GetPoint(position01);
		GetPoint(position11);
		
		float epsilon = 0.00001f;
		float alphaX = (aPosition.x - position00.x) / (position11.x - position00.x + epsilon);
		float alphaZ = (aPosition.z - position00.z) / (position11.z - position00.z + epsilon);

		CU::Vector3<float> returnPosition(aPosition);
		float lowerY = CU::Math::Lerp<float>(position00.y, position10.y, alphaX);
		float upperY = CU::Math::Lerp<float>(position01.y, position11.y, alphaX);

		returnPosition.y = CU::Math::Lerp<float>(lowerY, upperY, alphaZ);

		if (returnPosition.y < myIceHeight)
		{
			returnPosition.y = myIceHeight;
		}
		returnPosition.y += aHeightOffset;
		return returnPosition;
	}

	CU::Vector3<float> Terrain::CalcIntersection(const CU::Vector3<float>& aCameraPos
		, const CU::Vector3<float>& aRayCastToZero) const
	{
		CU::Vector3<float> toCamera(aCameraPos - aRayCastToZero);
		//float lengthToCamera = CU::Length(toCamera);
		CU::Normalize(toCamera);

		CU::Vector3<float> intersectionPosition(aRayCastToZero);
		int iterations = 0;
		while (GetAbove(intersectionPosition) == false && iterations < 100)
		{
			intersectionPosition += toCamera;
			++iterations;
		}

		return intersectionPosition;
	}

	void Terrain::CreateNavMesh()
	{
		myNavMesh = new Navigation::NavMesh();
	}

	void Terrain::LoadNavMesh(const std::string& aBinaryPath)
	{
		myNavMesh = new Navigation::NavMesh(aBinaryPath);
	}

	void Terrain::CreatePathFinder()
	{
		myPathFinder = new Navigation::PathFinderFunnel(myNavMesh);
	}

	void Terrain::CreateVertices()
	{
		myVertices.Init(myHeightMap->myWidth * myHeightMap->myDepth);
		myIndices.Init(myHeightMap->myWidth * myHeightMap->myDepth * 6);

		for (int z = 0; z < myHeightMap->myDepth; ++z)
		{
			for (int x = 0; x < myHeightMap->myWidth; ++x)
			{
				VertexPosNormUVBiTang vertex;
				vertex.myPosition.x = float(x) * mySize.x / float(myHeightMap->myWidth);
				vertex.myPosition.y = myHeightMap->myData[(myHeightMap->myDepth - (1 + z)) * myHeightMap->myWidth + x] * myHeight / 255.f;
				vertex.myPosition.z = float(z) * mySize.y / float(myHeightMap->myDepth);
				vertex.myUV.x = float(x) / float(myHeightMap->myWidth);
				vertex.myUV.y = float(1.f - z) / float(myHeightMap->myDepth);
				myVertices.Add(vertex);
			}
		}

		CalcNormals(myVertices);

		CU::Matrix33<float> rotationMatrix;
		rotationMatrix = rotationMatrix * CU::Matrix33<float>::CreateRotateAroundX(CU_PI / 2.f);
		rotationMatrix = rotationMatrix * CU::Matrix33<float>::CreateRotateAroundZ(CU_PI / 2.f);
		
		for (int i = 0; i < myVertices.Size(); ++i)
		{
			myVertices[i].myTangent = CU::GetNormalized(CU::Cross(myVertices[i].myNormal, myVertices[i].myNormal * rotationMatrix));
			myVertices[i].myBiNormal = CU::GetNormalized(CU::Cross(myVertices[i].myTangent, myVertices[i].myNormal));
		}

		for (int z = 0; z < myHeightMap->myDepth - 1; ++z)
		{
			for (int x = 0; x < myHeightMap->myWidth - 1; ++x)
			{
				//original clock-wise, seem to have to turn it (see below) remove comment when sorted out
				//indices.Add(z * myHeightMap->myWidth + x);
				//indices.Add(z * myHeightMap->myWidth + x + 1);
				//indices.Add((z + 1) * myHeightMap->myWidth + x);

				//indices.Add((z + 1) * myHeightMap->myWidth + x);
				//indices.Add(z * myHeightMap->myWidth + x + 1);
				//indices.Add((z + 1) * myHeightMap->myWidth + x + 1);

				myIndices.Add(z * myHeightMap->myWidth + x);
				myIndices.Add((z + 1) * myHeightMap->myWidth + x);
				myIndices.Add(z * myHeightMap->myWidth + x + 1);

				myIndices.Add((z + 1) * myHeightMap->myWidth + x);
				myIndices.Add((z + 1) * myHeightMap->myWidth + x + 1);
				myIndices.Add(z * myHeightMap->myWidth + x + 1);
			}
		}
	}

	void Terrain::CalcNormals(CU::GrowingArray<VertexPosNormUVBiTang>& someVertices) const
	{
		unsigned int height = myHeightMap->myDepth;
		unsigned int width = myHeightMap->myWidth;
		float yScale = myHeight / 255.f;
		yScale *= 0.2f;
		//float xScale = mySize.x / myHeightMap->myDepth;
		float xzScale = mySize.y / myHeightMap->myDepth;


		for (unsigned int y = 0; y<height; ++y)
		{
			for (unsigned int x = 0; x<width; ++x)
			{
				float sx = GetHeight(x<width - 1 ? x + 1 : x, y) - GetHeight(x == 0 ? x : x - 1, y);
				if (x == 0 || x == width - 1)
					sx *= 2;

				float sy = GetHeight(x, y<height - 1 ? y + 1 : y) - GetHeight(x, y == 0 ? y : y - 1);
				if (y == 0 || y == height - 1)
					sy *= 2;

				CU::Vector3<float> normal(-sx*xzScale, yScale, sy*xzScale);
				CU::Normalize(normal);
				normal.z = -normal.z;

				someVertices[y*width + x].myNormal = normal;
			}
		}
	}

	float Terrain::GetHeight(unsigned int aX, unsigned int aY) const
	{
		DL_ASSERT_EXP(aX < static_cast<unsigned int>(myHeightMap->myWidth), "X out of range");
		DL_ASSERT_EXP(aY < static_cast<unsigned int>(myHeightMap->myDepth), "Y out of range");
		return myHeightMap->myData[(myHeightMap->myDepth - (1 + aY)) * myHeightMap->myWidth + aX] / 255.f;
	}

	float Terrain::GetHeight(unsigned int aIndex) const
	{
		DL_ASSERT_EXP(aIndex < static_cast<unsigned int>(myHeightMap->myWidth * myHeightMap->myDepth), "index out of range");
		return myHeightMap->myData[aIndex] / 255.f;
	}

	void Terrain::GetPoint(CU::Vector3<float>& aPoint) const
	{
		aPoint.y = GetHeight(GetIndex(aPoint)) * myHeight;

		aPoint.x = floorf(aPoint.x / myCellSize) * myCellSize;
		aPoint.z = floorf(aPoint.z / myCellSize) * myCellSize;
	}

	int Terrain::GetIndex(const CU::Vector3<float>& aPosition) const
	{
		CU::Vector2<int> position(static_cast<unsigned int>(CU::Clip(aPosition.x / myCellSize, 0, myHeightMap->myWidth - 1.f))
			, static_cast<unsigned int>(CU::Clip(aPosition.z / myCellSize, 0, myHeightMap->myDepth - 1.f)));

		return (myHeightMap->myDepth - (1 + position.y)) * myHeightMap->myWidth + position.x;
	}

	bool Terrain::GetAbove(const CU::Vector3<float>& aPosition) const
	{
		return aPosition.y >= GetHeight(
			static_cast<unsigned int>(CU::Clip(aPosition.x / myCellSize, 0, myHeightMap->myWidth - 1.f))
			, static_cast<unsigned int>(CU::Clip(aPosition.z / myCellSize, 0, myHeightMap->myDepth - 1.f))) * myHeight;
	}
}