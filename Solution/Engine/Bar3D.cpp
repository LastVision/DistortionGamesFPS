#include "stdafx.h"
#include "Bar3D.h"
#include "Camera.h"
#include "IndexBufferWrapper.h"
#include <d3dx11effect.h>
#include "Surface.h"

namespace Prism
{
	Bar3D::Bar3D(const CU::Vector2<float>& aPositionFromJoint, const CU::Vector2<float>& aQuadSize
		, int aNumberOfQuads, Effect* aEffect)
		: myQuadSize(aQuadSize)
		, myValue(0.f)
		, myNbrOfQuads(aNumberOfQuads)
	{
		myEffect = aEffect;
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};


		InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "GUI::InputLayout");
		InitVertexBuffer(sizeof(GUIVertex), D3D11_USAGE_IMMUTABLE, 0);

		InitIndexBuffer();
		InitSurface("AlbedoTexture", "Data/Resource/Texture/T_missing_texture.dds");
		mySurfaces[0]->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		InitBlendState("GUI::BlendState");

		CreateVertices(aPositionFromJoint, aNumberOfQuads);
	}


	Bar3D::~Bar3D()
	{
	}

	void Bar3D::Render(const Camera& aCamera, const CU::Matrix44<float>& aWorld)
	{
		//Prism::Engine::GetInstance()->DisableCulling();
		myEffect->SetWorldMatrix(aWorld);
		myEffect->SetViewProjectionMatrix(aCamera.GetViewProjection());
		myEffect->SetCameraPosition(aCamera.GetOrientation().GetPos());
		//BaseModel::Render();
		Render();
		//Prism::Engine::GetInstance()->EnableCulling();
	}

	void Bar3D::Render()
	{

		Engine::GetInstance()->GetContex()->IASetInputLayout(myVertexLayout);
		Engine::GetInstance()->GetContex()->IASetVertexBuffers(myVertexBuffer->myStartSlot
			, myVertexBuffer->myNumberOfBuffers, &myVertexBuffer->myVertexBuffer
			, &myVertexBuffer->myStride, &myVertexBuffer->myByteOffset);
		Engine::GetInstance()->GetContex()->IASetIndexBuffer(myIndexBuffer->myIndexBuffer
			, myIndexBuffer->myIndexBufferFormat, myIndexBuffer->myByteOffset);

		for (int s = 0; s < mySurfaces.Size(); ++s)
		{
			int verticesToSend = abs(myValue) * myNbrOfQuads;
			int rest = verticesToSend * 2;
			verticesToSend = rest + 2;
			mySurfaces[s]->Activate();

			ID3DX11EffectTechnique* tech = nullptr;
			tech = myEffect->GetTechnique(myTechniqueName);

			if (tech->IsValid() == false)
			{
				tech = myEffect->GetTechnique();
				DL_ASSERT("INVALID TECHNIQUE IN BASEMODEL::RENDER: " + myTechniqueName);
			}

			DL_ASSERT_EXP(tech != nullptr, "Technique is nullptr");
			DL_ASSERT_EXP(tech->IsValid() != false, "Technique is not valid");

			D3DX11_TECHNIQUE_DESC techDesc;
			tech->GetDesc(&techDesc);
			for (UINT i = 0; i < techDesc.Passes; ++i)
			{
				tech->GetPassByIndex(i)->Apply(0, Engine::GetInstance()->GetContex());
				/*Engine::GetInstance()->GetContex()->DrawIndexed(indicesToSend
					, mySurfaces[s]->GetIndexStart(), 0);*/
				Engine::GetInstance()->GetContex()->Draw(verticesToSend, 0);
			}

			mySurfaces[s]->DeActivate();
		}
	}

	void Bar3D::SetValue(float aValue)
	{
		myValue = aValue;
	}

	void Bar3D::CreateVertices(const CU::Vector2<float>& aPositionFromJoint, int aNumberOfQuads)
	{
		CU::GrowingArray<GUIVertex> vertices(4 * aNumberOfQuads);
		//CU::GrowingArray<int> indices(6 * aNumberOfQuads);

		//for (int i = 0; i < aNumberOfQuads; ++i)
		//{
		//	int indiceCount = indices.Size();
		//	GUIVertex vertex;
		//	vertex.myPosition = { aPositionFromJoint.x, aPositionFromJoint.y + myQuadSize.y, 0, 0 };
		//	vertex.myPosition.y += myQuadSize.y * i;
		//	vertex.myUV = { 0, 1 };
		//	vertices.Add(vertex);

		//	vertex.myPosition = { aPositionFromJoint.x + myQuadSize.x, aPositionFromJoint.y + myQuadSize.y, 0, 0 };
		//	vertex.myPosition.y += myQuadSize.y * i;
		//	vertex.myUV = { 1, 1 };
		//	vertices.Add(vertex);

		//	vertex.myPosition = { aPositionFromJoint.x + myQuadSize.x, aPositionFromJoint.y, 0, 0 };
		//	vertex.myPosition.y += myQuadSize.y * i;
		//	vertex.myUV = { 1, 0 };
		//	vertices.Add(vertex);

		//	vertex.myPosition = { aPositionFromJoint.x, aPositionFromJoint.y, 0, 0 };
		//	vertex.myPosition.y += myQuadSize.y * i;
		//	vertex.myUV = { 0, 0 };
		//	vertices.Add(vertex);

		//	indices.Add(indiceCount);
		//	indices.Add(indiceCount + 1);
		//	indices.Add(indiceCount + 3);
		//				
		//	indices.Add(indiceCount + 1);
		//	indices.Add(indiceCount + 2);
		//	indices.Add(indiceCount + 3);
		//}

		for (int i = 0; i < aNumberOfQuads; ++i)
		{
			GUIVertex vertex;
			if (i == 0)
			{
				vertex.myPosition = { aPositionFromJoint.x + myQuadSize.x, aPositionFromJoint.y, 0, 0 };
				vertex.myPosition.y += myQuadSize.y * i;
				vertex.myUV = { 1, 0 };
				vertices.Add(vertex);

				vertex.myPosition = { aPositionFromJoint.x, aPositionFromJoint.y, 0, 0 };
				vertex.myPosition.y += myQuadSize.y * i;
				vertex.myUV = { 0, 0 };
				vertices.Add(vertex);

				vertex.myPosition = { aPositionFromJoint.x + myQuadSize.x, aPositionFromJoint.y + myQuadSize.y, 0, 0 };
				vertex.myPosition.y += myQuadSize.y * i;
				vertex.myUV = { 1, 1 };
				vertices.Add(vertex);

				vertex.myPosition = { aPositionFromJoint.x, aPositionFromJoint.y + myQuadSize.y, 0, 0 };
				vertex.myPosition.y += myQuadSize.y * i;
				vertex.myUV = { 0, 1 };
				vertices.Add(vertex);
			}
			else
			{
				vertex.myPosition = { aPositionFromJoint.x + myQuadSize.x, aPositionFromJoint.y, 0, 0 };
				vertex.myPosition.y += myQuadSize.y * i;
				vertex.myUV = { 1, 0 };
				vertices.Add(vertex);

				vertex.myPosition = { aPositionFromJoint.x, aPositionFromJoint.y, 0, 0 };
				vertex.myPosition.y += myQuadSize.y * i;
				vertex.myUV = { 0, 0 };
				vertices.Add(vertex);
			}

		}

		SetupVertexBuffer(vertices.Size(), sizeof(GUIVertex), reinterpret_cast<char*>(&vertices[0])
			, "GUI::VertexBuffer");
		//SetupIndexBuffer(indices.Size(), reinterpret_cast<char*>(&indices[0]), "GUI::IndexBuffer");

		mySurfaces[0]->SetVertexCount(vertices.Size());
		//mySurfaces[0]->SetIndexCount(indices.Size());
	}
}