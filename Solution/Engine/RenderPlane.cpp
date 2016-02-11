#include "stdafx.h"

#include "Camera.h"
#include "RenderPlane.h"
#include "Surface.h"
#include <d3dx11effect.h>


namespace Prism
{
	RenderPlane::RenderPlane(int aSubDivisions, float aTileSize)
		: mySubDivisions(aSubDivisions)
		, myTileSize(aTileSize)
	{
		myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_plane.fx");
		Surface* surface = new Surface();
		surface->SetEffect(myEffect);
		surface->SetIndexStart(0);
		surface->SetVertexStart(0);
		surface->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mySurfaces.Add(surface);

		myMappedResource = new D3D11_MAPPED_SUBRESOURCE();

		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};


		InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "RenderPlane::InputLayout");
		InitVertexBuffer(sizeof(VertexPosColor), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

		InitIndexBuffer();
		InitBlendState("RenderPlane::BlendState");

		ZeroMemory(myInitData, sizeof(*myInitData));

		CreateVertices();
	}


	RenderPlane::~RenderPlane()
	{
		SAFE_DELETE(myMappedResource);
	}

	void RenderPlane::Render(const Camera& aCamera, const CU::Matrix44<float>& aOrientation)
	{
		myEffect->SetWorldMatrix(aOrientation);
		myEffect->SetViewProjectionMatrix(aCamera.GetViewProjection());
		myEffect->SetCameraPosition(aCamera.GetOrientation().GetPos());

		BaseModel::Render(false);
	}

	void RenderPlane::StartModify()
	{
		Engine::GetInstance()->GetContex()->Map(myVertexBuffer->myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, myMappedResource);

		if (myMappedResource->pData == nullptr)
		{
			DL_ASSERT("RenderPlane: Failed to StartModify, mapping failed");
		}
	}

	void RenderPlane::EndModify()
	{
		if (myMappedResource->pData == nullptr)
		{
			DL_ASSERT("RenderPlane: Need to StartModify before EndModify");
		}

		VertexPosColor* data = (VertexPosColor*)myMappedResource->pData;
		memcpy(data, &myVertices[0], sizeof(VertexPosColor) * myVertices.Size());
		Engine::GetInstance()->GetContex()->Unmap(myVertexBuffer->myVertexBuffer, 0);
	}

	void RenderPlane::SetVertexColor(int aIndex, const CU::Vector4<float>& aColor)
	{
		int baseIndex = aIndex * 4;
		myVertices[baseIndex].myCol = aColor;
		myVertices[baseIndex+1].myCol = aColor;
		myVertices[baseIndex+2].myCol = aColor;
		myVertices[baseIndex+3].myCol = aColor;
	}

	int RenderPlane::GetVertexCount() const
	{
		return myVertices.Size();
	}

	void RenderPlane::CreateVertices()
	{
		myVertices.Init(mySubDivisions * mySubDivisions);
		CU::GrowingArray<int> indices(128);
		VertexPosColor vertex;
		vertex.myCol = { 1.f, 1.f, 1.f, 1.f };
		int index = 0;

		for (int y = 0; y < mySubDivisions; ++y)
		{
			for (int x = 0; x < mySubDivisions; ++x)
			{
				//TOPLEFT
				vertex.myPos = { x * myTileSize, 0, y * myTileSize, 0.f };
				myVertices.Add(vertex);

				//TOPRIGHT
				vertex.myPos = { (x + 1) * myTileSize, 0, y * myTileSize, 0.f };
				myVertices.Add(vertex);

				//BOTRIGHT
				vertex.myPos = { (x + 1) * myTileSize, 0, (y + 1) * myTileSize, 0.f };
				myVertices.Add(vertex);

				//BOTLEFT
				vertex.myPos = { x * myTileSize, 0, (y + 1) * myTileSize, 0.f };
				myVertices.Add(vertex);


				indices.Add(index + 3);
				indices.Add(index + 1);
				indices.Add(index + 0);

				indices.Add(index + 2);
				indices.Add(index + 1);
				indices.Add(index + 3);
				index += 4;
			}
		}

		SetupVertexBuffer(myVertices.Size(), sizeof(VertexPosColor)
			, reinterpret_cast<char*>(&myVertices[0]), "RenderPlane::VertexBuffer");
		SetupIndexBuffer(indices.Size(), reinterpret_cast<char*>(&indices[0]), "RenderPlane::IndexBuffer");

		mySurfaces[0]->SetVertexCount(myVertices.Size());
		mySurfaces[0]->SetIndexCount(indices.Size());
	}
}