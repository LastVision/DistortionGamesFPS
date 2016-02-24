#include "stdafx.h"
#include "Bar3D.h"
#include "Camera.h"
#include <d3dx11effect.h>
#include "Surface.h"

namespace Prism
{
	Bar3D::Bar3D(const CU::Vector2<float>& aPositionFromJoint, const CU::Vector2<float>& aQuadSize
		, int aNumberOfQuads, Effect* aEffect)
		: myQuadSize(aQuadSize)
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
		BaseModel::Render();
		//Prism::Engine::GetInstance()->EnableCulling();
	}

	void Bar3D::CreateVertices(const CU::Vector2<float>& aPositionFromJoint, int aNumberOfQuads)
	{
		CU::GrowingArray<GUIVertex> vertices(4);
		CU::GrowingArray<int> indices(6);


		GUIVertex vertex;
		vertex.myPosition = { aPositionFromJoint.x, aPositionFromJoint.y + myQuadSize.y, 0, 0 };
		vertex.myUV = { 0, 1 };
		vertices.Add(vertex);

		vertex.myPosition = { aPositionFromJoint.x + myQuadSize.x, aPositionFromJoint.y + myQuadSize.y, 0, 0 };
		vertex.myUV = { 1, 1 };
		vertices.Add(vertex);

		vertex.myPosition = { aPositionFromJoint.x + myQuadSize.x, aPositionFromJoint.y, 0, 0 };
		vertex.myUV = { 1, 0 };
		vertices.Add(vertex);

		vertex.myPosition = { aPositionFromJoint.x, aPositionFromJoint.y, 0, 0 };
		vertex.myUV = { 0, 0 };
		vertices.Add(vertex);

		indices.Add(0);
		indices.Add(1);
		indices.Add(3);

		indices.Add(1);
		indices.Add(2);
		indices.Add(3);

		SetupVertexBuffer(vertices.Size(), sizeof(GUIVertex), reinterpret_cast<char*>(&vertices[0])
			, "GUI::VertexBuffer");
		SetupIndexBuffer(indices.Size(), reinterpret_cast<char*>(&indices[0]), "GUI::IndexBuffer");

		mySurfaces[0]->SetVertexCount(vertices.Size());
		mySurfaces[0]->SetIndexCount(indices.Size());
	}
}