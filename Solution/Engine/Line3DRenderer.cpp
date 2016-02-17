#include "stdafx.h"

#include "Camera.h"
#include <d3dx11effect.h>
#include "EffectContainer.h"
#include "IndexBufferWrapper.h"
#include "Line3D.h"
#include "Line3DRenderer.h"
#include "ModelLoader.h"
namespace Prism
{
	Line3DRenderer::Line3DRenderer()
		: myEffect(nullptr)
		, myVertexBuffer(nullptr)
		, myVertexBufferDesc(new D3D11_BUFFER_DESC())
		, myInputLayout(nullptr)
		, myIndexBufferDesc(nullptr)
	{
		myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		ModelLoader::GetInstance()->Pause();
		InitVertexBuffer();
		myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_line3d.fx");
		CreateInputLayout();
		ModelLoader::GetInstance()->UnPause();
	}

	Line3DRenderer::~Line3DRenderer()
	{
		SAFE_DELETE(myVertexBuffer);
		SAFE_DELETE(myVertexBufferDesc);
		if (myInputLayout != nullptr)
		{
			myInputLayout->Release();
			myInputLayout = nullptr;
		}
	}

	void Line3DRenderer::Render(const CU::GrowingArray<Line3D>& someLines, const Camera& aCamera)
	{ 
		if (someLines.Size() > 0)
		{
			UpdateVertexBuffer(someLines);

			myEffect->SetWorldMatrix(CU::Matrix44<float>());
			myEffect->SetViewMatrix(CU::InverseSimple(aCamera.GetOrientation()));
			myEffect->SetProjectionMatrix(aCamera.GetProjection());

			Engine::GetInstance()->GetContex()->IASetInputLayout(myInputLayout);
			Engine::GetInstance()->GetContex()->IASetPrimitiveTopology(myPrimitiveTopology);
			Engine::GetInstance()->GetContex()->IASetVertexBuffers(
				myVertexBuffer->myStartSlot
				, myVertexBuffer->myNumberOfBuffers
				, &myVertexBuffer->myVertexBuffer
				, &myVertexBuffer->myStride
				, &myVertexBuffer->myByteOffset);

			D3DX11_TECHNIQUE_DESC techDesc;
			myEffect->GetTechnique(false)->GetDesc(&techDesc);

			for (UINT i = 0; i < techDesc.Passes; ++i)
			{
				myEffect->GetTechnique(false)->GetPassByIndex(i)->Apply(0, Engine::GetInstance()->GetContex());
				Engine::GetInstance()->GetContex()->Draw(someLines.Size() * 2, 0);
			}
		}
	}

	void Line3DRenderer::UpdateVertexBuffer(const CU::GrowingArray<Line3D>& someLines)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		Engine::GetInstance()->GetContex()->Map(myVertexBuffer->myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (mappedResource.pData != nullptr)
		{
			Line3DVertex* data = (Line3DVertex*)mappedResource.pData;

			bool isSafe = sizeof(Line3DVertex) == sizeof(someLines[0].myFirstPoint);
			DL_ASSERT_EXP(isSafe, "[Line3DRenderer](UpdateVertexBuffer) : Not safe to copy.");
			memcpy(data, &someLines[0], sizeof(Line3D) * someLines.Size());
		}
		Engine::GetInstance()->GetContex()->Unmap(myVertexBuffer->myVertexBuffer, 0);
	}


	void Line3DRenderer::CreateInputLayout()
	{
		HRESULT hr;

		D3DX11_PASS_DESC passDesc;
		hr = myEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
		DL_ASSERT_EXP(!FAILED(hr), "[Line3DRenderer](CreateInputLayout) : Failed to get Pass Description!");

		const D3D11_INPUT_ELEMENT_DESC Line3DRendererLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		UINT size = ARRAYSIZE(Line3DRendererLayout);

		hr = Engine::GetInstance()->GetDevice()->CreateInputLayout(Line3DRendererLayout
			, size
			, passDesc.pIAInputSignature
			, passDesc.IAInputSignatureSize
			, &myInputLayout);
		DL_ASSERT_EXP(!FAILED(hr), "[Line3DRenderer](CreateInputLayout) : Failed to Create InputLayout!");

		Engine::GetInstance()->SetDebugName(myInputLayout, "Line3DRenderer::myInputLayout");
	}

	void Line3DRenderer::InitVertexBuffer()
	{
		myVertexBuffer = new VertexBufferWrapper();
		myVertexBuffer->myStride = sizeof(Line3DVertex);;
		myVertexBuffer->myByteOffset = 0;
		myVertexBuffer->myStartSlot = 0;
		myVertexBuffer->myNumberOfBuffers = 1;


		ZeroMemory(myVertexBufferDesc, sizeof(*myVertexBufferDesc));
		myVertexBufferDesc->Usage = D3D11_USAGE_DYNAMIC;
		myVertexBufferDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;
		myVertexBufferDesc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		myVertexBufferDesc->MiscFlags = 0;
		myVertexBufferDesc->StructureByteStride = 0;


		HRESULT hr;

		if (myVertexBuffer->myVertexBuffer != nullptr)
		{
			myVertexBuffer->myVertexBuffer->Release();
		}

		myVertexBufferDesc->ByteWidth = (sizeof(Line3DVertex) * 2) * MAXNROFLINES;

		hr = Engine::GetInstance()->GetDevice()->CreateBuffer(myVertexBufferDesc, nullptr, &myVertexBuffer->myVertexBuffer);
		DL_ASSERT_EXP(hr == S_OK, "[Line3DRenderer](CreateVertexBuffer) : Failed to create VertexBuffer");

		Engine::GetInstance()->SetDebugName(myVertexBuffer->myVertexBuffer
			, "Line3DRenderer::myVertexWrapper->myVertexBuffer");
	}

	//void Line3DRenderer::InitIndexBuffer()
	//{
	//	myIndexBuffer = new IndexBufferWrapper();
	//	myIndexBuffer->myIndexBufferFormat = DXGI_FORMAT_R32_UINT;
	//	myIndexBuffer->myByteOffset = 0;


	//	ZeroMemory(myIndexBufferDesc, sizeof(myIndexBufferDesc));
	//	myIndexBufferDesc->Usage = D3D11_USAGE_IMMUTABLE;
	//	myIndexBufferDesc->BindFlags = D3D11_BIND_INDEX_BUFFER;
	//	myIndexBufferDesc->CPUAccessFlags = 0;
	//	myIndexBufferDesc->MiscFlags = 0;
	//	myIndexBufferDesc->StructureByteStride = 0;
	//}
}