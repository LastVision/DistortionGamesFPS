#include "stdafx.h"
#include "Camera.h"
#include <d3dx11effect.h>
#include "SelectionCircle.h"
#include <MathHelper.h>
#include "Surface.h"
#include "TextureHelper.h"
#include "VertexIndexWrapper.h"
#include "VertexDataWrapper.h"
#include "Terrain.h"

namespace Prism
{
	SelectionCircle::SelectionCircle(Effect* aEffect, const CU::Vector2<float>& aSize, float aHeight, const std::string& aFilePath
		, const Terrain& aTerrain)
		: mySize(aSize)
		, myHeight(aHeight)
		, myVertices(8)
		, myTerrain(aTerrain)
	{
		myEffect = aEffect;


		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};


		InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "SelectionCircle::InputLayout");
		InitVertexBuffer(sizeof(IceVertex), D3D11_USAGE_DYNAMIC, 0);

		InitIndexBuffer();
		InitSurface("AlbedoTexture", aFilePath);

		InitBlendState("SelectionCircle::BlendState");

		ZeroMemory(myInitData, sizeof(*myInitData));

		CreateVertices();

	}

	SelectionCircle::~SelectionCircle()
	{
		SAFE_DELETE(myIndexBaseData);
		SAFE_DELETE(myVertexBaseData);
	}

	void SelectionCircle::Render(const Camera& aCamera, const CU::Matrix44<float>& aOrientation)
	{
		myEffect->SetWorldMatrix(aOrientation);
		myEffect->SetViewProjectionMatrix(aCamera.GetViewProjection());
		myEffect->SetCameraPosition(aCamera.GetOrientation().GetPos());


		D3D11_MAPPED_SUBRESOURCE mappedResource;
		Engine::GetInstance()->GetContex()->Map(myVertexBuffer->myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (mappedResource.pData != nullptr)
		{
			IceVertex* data = (IceVertex*)mappedResource.pData;

			for (int i = 0; i < myVertices.Size(); ++i)
			{
				myVertices[i].myPosition.y = myHeight + myTerrain.GetHeight(CU::Vector2<float>(aOrientation.GetPos().x, aOrientation.GetPos().z)).y;
			}



			memcpy(data, &myVertexBuffer[0], sizeof(IceVertex) * 4);


		}
		Engine::GetInstance()->GetContex()->Unmap(myVertexBuffer->myVertexBuffer, 0);
		

		BaseModel::Render(false);
	}

	void SelectionCircle::CreateVertices()
	{
		CU::GrowingArray<IceVertex> vertices(4);
		CU::GrowingArray<int> indices(6);


		IceVertex vertex;
		vertex.myPosition = { 0, myHeight, 0, 0 };
		vertex.myNormal = { 0, 1, 0, 0 };
		vertex.myUV = { 0, 1 };
		myVertices.Add(vertex);

		vertex.myPosition = { mySize.x, myHeight, 0, 0 };
		vertex.myNormal = { 0, 1, 0, 0 };
		vertex.myUV = { 1, 1 };
		myVertices.Add(vertex);

		vertex.myPosition = { mySize.x, myHeight, mySize.y, 0 };
		vertex.myNormal = { 0, 1, 0, 0 };
		vertex.myUV = { 1, 0 };
		myVertices.Add(vertex);

		vertex.myPosition = { 0, myHeight, mySize.y, 0 };
		vertex.myNormal = { 0, 1, 0, 0 };
		vertex.myUV = { 0, 0 };
		myVertices.Add(vertex);


		for (int i = 0; i < vertices.Size(); ++i)
		{
			myVertices[i].myTangent = CU::Vector4<float>(1.f, 0.f, 0.f, 0);
			myVertices[i].myBiNormal = CU::Vector4<float>(0.f, 0.f, 1.f, 0);
		}


		indices.Add(3);
		indices.Add(1);
		indices.Add(0);

		indices.Add(2);
		indices.Add(1);
		indices.Add(3);

		SetupVertexBuffer(myVertices.Size(), sizeof(IceVertex), reinterpret_cast<char*>(&myVertices[0])
			, "SelectionCircle::VertexBuffer");
		SetupIndexBuffer(indices.Size(), reinterpret_cast<char*>(&indices[0]), "SelectionCircle::IndexBuffer");

		mySurfaces[0]->SetVertexCount(myVertices.Size());
		mySurfaces[0]->SetIndexCount(indices.Size());
	}
}