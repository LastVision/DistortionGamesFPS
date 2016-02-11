#include "stdafx.h"
#include "Camera.h"
#include "Cube3D.h"
#include <d3dx11effect.h>
#include "Engine.h"
#include "DirectX.h"
#include "EffectContainer.h"
#include "ModelLoader.h"
#include "VertexDataWrapper.h"
#include "VertexIndexWrapper.h"
#include "IndexBufferWrapper.h"

namespace Prism
{
	Cube3D::Cube3D()
		: myWireFrame(false)
		, mySize(1.f, 1.f, 1.f)
		, myColor(1.f, 1.f, 1.f, 1.f)
	{
		myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_cube3d.fx");
		CreateVertexBuffer(1.f);

		myIndexBaseData = new VertexIndexWrapper();
		myIndexBaseData->myFormat = DXGI_FORMAT_R32_UINT;
		myIndexBaseData->myNumberOfIndices = 6 * 6;
		myIndexBaseData->mySize = myIndexBaseData->myNumberOfIndices * sizeof(unsigned int);


		CU::GrowingArray<int> indices;
		indices.Init(32);

		indices.Add(3);
		indices.Add(1);
		indices.Add(0);

		indices.Add(2);
		indices.Add(1);
		indices.Add(3);

		indices.Add(6);
		indices.Add(4);
		indices.Add(5);

		indices.Add(6);
		indices.Add(7);
		indices.Add(4);

		indices.Add(11);
		indices.Add(9);
		indices.Add(8);

		indices.Add(10);
		indices.Add(9);
		indices.Add(11);

		indices.Add(14);
		indices.Add(12);
		indices.Add(13);

		indices.Add(15);
		indices.Add(12);
		indices.Add(14);

		indices.Add(19);
		indices.Add(17);
		indices.Add(16);

		indices.Add(18);
		indices.Add(17);
		indices.Add(19);

		indices.Add(22);
		indices.Add(20);
		indices.Add(21);

		indices.Add(23);
		indices.Add(20);
		indices.Add(22);

		myIndexBaseData->myIndexData = new char[myIndexBaseData->mySize]();
		memcpy(myIndexBaseData->myIndexData, &indices[0], indices.Size() * sizeof(unsigned int));

		myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		InitVertexBuffer();
		InitIndexBuffer();
	}


	void Cube3D::CreateVertexBuffer(float aSideLength)
	{
		VertexPosNormColor temp;

		float offset = aSideLength * 0.5f;

		CU::GrowingArray<VertexPosNormColor> vertices;
		vertices.Init(16);

		temp.myPosition = { -offset, offset, -offset, 1.f };
		temp.myNormal = { 0.0f, 1.0f, 0.0f, 0.f };
		temp.myColor = myColor;
		vertices.Add(temp);

		temp.myPosition = { offset, offset, -offset, 1.f };
		temp.myNormal = { 0.0f, 1.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, offset, offset, 1.f };
		temp.myNormal = { 0.0f, 1.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, offset, offset, 1.f };
		temp.myNormal = { 0.0f, 1.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, -offset, -offset, 1.f };
		temp.myNormal = { 0.0f, -1.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, -offset, -offset, 1.f };
		temp.myNormal = { 0.0f, -1.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, -offset, offset, 1.f };
		temp.myNormal = { 0.0f, -1.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, -offset, offset, 1.f };
		temp.myNormal = { 0.0f, -1.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, -offset, offset, 1.f };
		temp.myNormal = { -1.0f, 0.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, -offset, -offset, 1.f };
		temp.myNormal = { -1.0f, 0.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, offset, -offset, 1.f };
		temp.myNormal = { -1.0f, 0.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, offset, offset, 1.f };
		temp.myNormal = { -1.0f, 0.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, -offset, offset, 1.f };
		temp.myNormal = { 1.0f, 0.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, -offset, -offset, 1.f };
		temp.myNormal = { 1.0f, 0.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, offset, -offset, 1.f };
		temp.myNormal = { 1.0f, 0.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, offset, offset, 1.f };
		temp.myNormal = { 1.0f, 0.0f, 0.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, -offset, -offset, 1.f };
		temp.myNormal = { 0.0f, 0.0f, -1.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, -offset, -offset, 1.f };
		temp.myNormal = { 0.0f, 0.0f, -1.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, offset, -offset, 1.f };
		temp.myNormal = { 0.0f, 0.0f, -1.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, offset, -offset, 1.f };
		temp.myNormal = { 0.0f, 0.0f, -1.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, -offset, offset, 1.f };
		temp.myNormal = { 0.0f, 0.0f, 1.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, -offset, offset, 1.f };
		temp.myNormal = { 0.0f, 0.0f, 1.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { offset, offset, offset, 1.f };
		temp.myNormal = { 0.0f, 0.0f, 1.0f, 0.f };
		vertices.Add(temp);

		temp.myPosition = { -offset, offset, offset, 1.f };
		temp.myNormal = { 0.0f, 0.0f, 1.0f, 0.f };
		vertices.Add(temp);
		myVertexBaseData = new VertexDataWrapper();
		myVertexBaseData->myNumberOfVertices = vertices.Size();
		myVertexBaseData->myStride = sizeof(VertexPosNormColor);
		myVertexBaseData->mySize = myVertexBaseData->myNumberOfVertices * myVertexBaseData->myStride;
		myVertexBaseData->myType = VertexType::POS_NORM_COLOR;
		myVertexBaseData->myVertexData = new char[myVertexBaseData->mySize]();
		memcpy(myVertexBaseData->myVertexData, &vertices[0], myVertexBaseData->mySize);
	}


	Cube3D::~Cube3D()
	{
		SAFE_RELEASE(myVertexBuffer->myVertexBuffer);
		SAFE_RELEASE(myIndexBuffer->myIndexBuffer);
		SAFE_DELETE(myVertexBaseData);
		SAFE_DELETE(myVertexBuffer);
		SAFE_DELETE(myIndexBaseData);
		SAFE_DELETE(myIndexBuffer);
		SAFE_RELEASE(myInputLayout);
	}

	void Cube3D::SetSizeAndColor(float aSideLength, const CU::Vector4<float>& aColor)
	{
		mySize.x = aSideLength;
		mySize.y = aSideLength;
		mySize.z = aSideLength;
		myColor = aColor;
	}

	void Cube3D::InitVertexBuffer()
	{
		myVertexBuffer = new VertexBufferWrapper();
		HRESULT hr;

		if (myInputLayout == nullptr)
		{
			D3DX11_PASS_DESC passDesc;
			hr = myEffect->GetTechnique(false)->GetPassByIndex(0)->GetDesc(&passDesc);
			DL_ASSERT_EXP(!FAILED(hr), "[Cube3D](CreateInputLayout) : Failed to get Pass Description!");

			const D3D11_INPUT_ELEMENT_DESC Cube3DRendererLayout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			UINT size = ARRAYSIZE(Cube3DRendererLayout);

			hr = Engine::GetInstance()->GetDevice()->CreateInputLayout(Cube3DRendererLayout
				, size
				, passDesc.pIAInputSignature
				, passDesc.IAInputSignatureSize
				, &myInputLayout);
			DL_ASSERT_EXP(!FAILED(hr), "[Cube3D](CreateInputLayout) : Failed to Create InputLayout!");

			Engine::GetInstance()->SetDebugName(myInputLayout, "Cube3D::myInputLayout");
		}

		D3D11_BUFFER_DESC vertexBufferDescription;
		vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDescription.ByteWidth = myVertexBaseData->mySize;
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDescription.MiscFlags = 0;
		vertexBufferDescription.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferInitData;
		vertexBufferInitData.pSysMem = myVertexBaseData->myVertexData;

		// Set vertex buffer
		hr = Engine::GetInstance()->GetDevice()->CreateBuffer(&vertexBufferDescription, &vertexBufferInitData
			, &myVertexBuffer->myVertexBuffer);
		if (FAILED(hr))
		{
			DL_ASSERT("Failed to create vertexbuffer in CUBE3D");
		}

		Engine::GetInstance()->SetDebugName(myVertexBuffer->myVertexBuffer, "Cube3D::myVertexBuffer");


	}

	void Cube3D::InitIndexBuffer()
	{
		myIndexBuffer = new IndexBufferWrapper();
		D3D11_BUFFER_DESC indexBufferDescription;
		indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDescription.ByteWidth = myIndexBaseData->mySize;
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDescription.CPUAccessFlags = 0;
		indexBufferDescription.MiscFlags = 0;
		indexBufferDescription.StructureByteStride = 0;


		D3D11_SUBRESOURCE_DATA indexBufferInitData;
		indexBufferInitData.pSysMem = myIndexBaseData->myIndexData;
		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateBuffer(&indexBufferDescription, &indexBufferInitData
			, &myIndexBuffer->myIndexBuffer);
		if (FAILED(hr))
		{
			DL_ASSERT("FAILED TO CREATE INDEXBUFFER IN CUBE3D");
		}

		Engine::GetInstance()->SetDebugName(myIndexBuffer->myIndexBuffer, "Cube3D::myIndexBuffer");

	}

	void Cube3D::Render(const Camera& aCamera)
	{
		myEffect->SetWorldMatrix(myOrientation);
		myEffect->SetViewMatrix(CU::InverseSimple(aCamera.GetOrientation()));
		myEffect->SetProjectionMatrix(aCamera.GetProjection());
		myEffect->SetScaleVector(mySize);
		myEffect->SetColor(myColor);

		unsigned int stride = myVertexBaseData->myStride;
		unsigned int offset = 0;
		Engine::GetInstance()->GetContex()->IASetInputLayout(myInputLayout);
		Engine::GetInstance()->GetContex()->IASetVertexBuffers(0, 1, &myVertexBuffer->myVertexBuffer, &stride, &offset);
		Engine::GetInstance()->GetContex()->IASetIndexBuffer(myIndexBuffer->myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);



		D3DX11_TECHNIQUE_DESC techDesc;
		myEffect->GetTechnique(false)->GetDesc(&techDesc);

		Engine::GetInstance()->GetContex()->IASetPrimitiveTopology(myPrimitiveTopology);
		if (myWireFrame == true)
		{
			for (UINT p = 0; p < techDesc.Passes; ++p)
			{
				myEffect->GetTechnique(false)->GetPassByIndex(p)->Apply(0, Engine::GetInstance()->GetContex());
				Engine::GetInstance()->EnableWireframe();
				Engine::GetInstance()->GetContex()->DrawIndexed(myIndexBaseData->myNumberOfIndices, 0, 0);
				Engine::GetInstance()->DisableWireframe();
			}
		}
		else
		{
			for (UINT p = 0; p < techDesc.Passes; ++p)
			{
				myEffect->GetTechnique(false)->GetPassByIndex(p)->Apply(0, Engine::GetInstance()->GetContex());
				Engine::GetInstance()->GetContex()->DrawIndexed(myIndexBaseData->myNumberOfIndices, 0, 0);
			}
		}
	}
}