#include "stdafx.h"

#include "BaseModel.h"
#include <D3D11.h>
#include <d3dx11effect.h>
#include "Effect.h"
#include "IndexBufferWrapper.h"
#include "Surface.h"


namespace Prism
{

	BaseModel::BaseModel()
		: myTechniqueName("Render")
		, myVertexFormat(8)
		, myVertexLayout(nullptr)
	{
		myVertexBufferDesc = new D3D11_BUFFER_DESC();
		myIndexBufferDesc = new D3D11_BUFFER_DESC();
		myInitData = new D3D11_SUBRESOURCE_DATA();

		myEffect = nullptr;

		mySurfaces.Init(1);

		myVertexBuffer = nullptr;
		myIndexBuffer = nullptr;
		myBlendState = nullptr;

	}

	BaseModel::~BaseModel()
	{
		myVertexFormat.DeleteAll();
		if (myVertexBuffer != nullptr && myVertexBuffer->myVertexBuffer != nullptr)
		{
			myVertexBuffer->myVertexBuffer->Release();
			delete myVertexBuffer;
		}

		if (myIndexBuffer != nullptr && myIndexBuffer->myIndexBuffer != nullptr)
		{
			myIndexBuffer->myIndexBuffer->Release();
			delete myIndexBuffer;
		}

		if (myVertexLayout != nullptr)
		{
			myVertexLayout->Release();
			myVertexLayout = nullptr;
		}

		if (myBlendState != nullptr)
		{
			myBlendState->Release();
			myBlendState = nullptr;
		}

		delete myVertexBufferDesc;
		delete myIndexBufferDesc;
		delete myInitData;
		mySurfaces.DeleteAll();
	}

	Effect* BaseModel::GetEffect()
	{
		return myEffect;
	}


	void BaseModel::SetTechniqueName(const std::string& aName)
	{
		myTechniqueName = aName;
	}

	void BaseModel::EvaluateEffectTechnique(bool aInstanced)
	{
		int uvCount = 0;
		bool hasVertexColor = false;
		for (int i = 0; i < myVertexFormat.Size(); ++i)
		{
			std::string semanticName(myVertexFormat[i]->SemanticName);
			if (semanticName == "TEXCOORD")
			{
				++uvCount;
			}
			if (semanticName == "COLOR")
			{
				hasVertexColor = true;
			}
		}

		if (hasVertexColor == true)
		{
			if (uvCount == 2)
			{
				myTechniqueName = "Render_2UVSET_COLOR";

			}
			else if (uvCount == 1)
			{
				myTechniqueName = "Render_1UVSET_COLOR";
			}
			else
			{
				DL_ASSERT("Model EvaluateTechnique: invalid uv-set-count with vertexcolor.");
			}
		}
		else
		{
			if (uvCount == 2)
			{
				myTechniqueName = "Render_2UVSET";

			}
			else if (uvCount == 1)
			{
				myTechniqueName = "Render_1UVSET";
			}
			else
			{
				DL_ASSERT("Model EvaluateTechnique: invalid uv-set-count without vertexcolor.");
			}
		}

		
		if (aInstanced == true)
		{
			myTechniqueName += "_INSTANCED";
		}
	}

	void BaseModel::Render()
	{
		Engine::GetInstance()->GetContex()->IASetInputLayout(myVertexLayout);
		Engine::GetInstance()->GetContex()->IASetVertexBuffers(myVertexBuffer->myStartSlot
			, myVertexBuffer->myNumberOfBuffers, &myVertexBuffer->myVertexBuffer
			, &myVertexBuffer->myStride, &myVertexBuffer->myByteOffset);
		Engine::GetInstance()->GetContex()->IASetIndexBuffer(myIndexBuffer->myIndexBuffer
			, myIndexBuffer->myIndexBufferFormat, myIndexBuffer->myByteOffset);

		//bool usePixelShader = Engine::GetInstance()->UsePBLPixelShader();

		for (int s = 0; s < mySurfaces.Size(); ++s)
		{
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
				Engine::GetInstance()->GetContex()->DrawIndexed(mySurfaces[s]->GetIndexCount()
					, mySurfaces[s]->GetIndexStart(), 0);
			}

			mySurfaces[s]->DeActivate();
		}
	}

	void BaseModel::InitInputLayout(D3D11_INPUT_ELEMENT_DESC* aVertexDescArray, int aArraySize, const std::string& aDebugName)
	{
		D3DX11_PASS_DESC passDesc;
		myEffect->GetTechnique(myTechniqueName)->GetPassByIndex(0)->GetDesc(&passDesc);
		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateInputLayout(aVertexDescArray
			, aArraySize, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &myVertexLayout);
		if (FAILED(hr) != S_OK)
		{
			DL_MESSAGE_BOX("Failed to CreateInputLayout", "BaseModel::Init", MB_ICONWARNING);
		}

		Engine::GetInstance()->SetDebugName(myVertexLayout, aDebugName);
	}

	void BaseModel::InitVertexBuffer(int aVertexSize, int aBufferUsage, int aCPUUsage)
	{
		myVertexBuffer = new VertexBufferWrapper();
		myVertexBuffer->myStride = aVertexSize;
		myVertexBuffer->myByteOffset = 0;
		myVertexBuffer->myStartSlot = 0;
		myVertexBuffer->myNumberOfBuffers = 1;


		ZeroMemory(myVertexBufferDesc, sizeof(*myVertexBufferDesc));
		myVertexBufferDesc->Usage = static_cast<D3D11_USAGE>(aBufferUsage);
		myVertexBufferDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;
		myVertexBufferDesc->CPUAccessFlags = aCPUUsage;
		myVertexBufferDesc->MiscFlags = 0;
		myVertexBufferDesc->StructureByteStride = 0;
	}

	void BaseModel::InitIndexBuffer()
	{
		myIndexBuffer = new IndexBufferWrapper();
		myIndexBuffer->myIndexBufferFormat = DXGI_FORMAT_R32_UINT;
		myIndexBuffer->myByteOffset = 0;


		ZeroMemory(myIndexBufferDesc, sizeof(*myIndexBufferDesc));
		myIndexBufferDesc->Usage = D3D11_USAGE_IMMUTABLE;
		myIndexBufferDesc->BindFlags = D3D11_BIND_INDEX_BUFFER;
		myIndexBufferDesc->CPUAccessFlags = 0;
		myIndexBufferDesc->MiscFlags = 0;
		myIndexBufferDesc->StructureByteStride = 0;
	}

	void BaseModel::InitSurface(const std::string& aResourceName, const std::string& aFileName)
	{
		Surface* surface = new Surface();

		surface->SetIndexCount(0);
		surface->SetIndexStart(0);
		surface->SetVertexCount(0);
		surface->SetVertexStart(0);
		surface->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		surface->SetTexture(aResourceName, aFileName, true);
		surface->SetEffect(myEffect);
		mySurfaces.Add(surface);
	}

	void BaseModel::InitSurface(const std::string& aResourceName, ID3D11ShaderResourceView* aShaderView)
	{
		Surface* surface = new Surface();

		surface->SetEffect(myEffect);
		surface->SetIndexCount(0);
		surface->SetIndexStart(0);
		surface->SetVertexCount(0);
		surface->SetVertexStart(0);
		surface->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		surface->SetTexture(aResourceName, aShaderView);
		mySurfaces.Add(surface);
	}

	void BaseModel::InitBlendState(const std::string& aDebugName)
	{
		D3D11_BLEND_DESC blendDesc;
		blendDesc.AlphaToCoverageEnable = true;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateBlendState(&blendDesc, &myBlendState);
		if (FAILED(hr) != S_OK)
		{
			DL_ASSERT("BaseModel::InitBlendState: Failed to CreateBlendState");
		}

		Engine::GetInstance()->SetDebugName(myBlendState, aDebugName);
	}

	void BaseModel::SetupVertexBuffer(int aVertexCount, int aVertexSize, char* aVertexData, const std::string& aDebugName)
	{
		if (myVertexBuffer->myVertexBuffer != nullptr)
			myVertexBuffer->myVertexBuffer->Release();

		myVertexBufferDesc->ByteWidth = aVertexSize * aVertexCount;

		HRESULT hr;
		if (aVertexData != nullptr)
		{
			myInitData->pSysMem = aVertexData;
			hr = Engine::GetInstance()->GetDevice()->CreateBuffer(myVertexBufferDesc, myInitData
				, &myVertexBuffer->myVertexBuffer);
		}
		else
		{
			hr = Engine::GetInstance()->GetDevice()->CreateBuffer(myVertexBufferDesc, NULL
				, &myVertexBuffer->myVertexBuffer);
		}

		if (FAILED(hr) != S_OK)
		{
			DL_ASSERT("BaseModel::SetupVertexBuffer: Failed to SetupVertexBuffer");
		}

		Engine::GetInstance()->SetDebugName(myVertexBuffer->myVertexBuffer, aDebugName);
	}

	void BaseModel::SetupIndexBuffer(int aIndexCount, char* aIndexData, const std::string& aDebugName)
	{
		if (myIndexBuffer->myIndexBuffer != nullptr)
			myIndexBuffer->myIndexBuffer->Release();

		myIndexBufferDesc->ByteWidth = sizeof(UINT) * aIndexCount;
		myInitData->pSysMem = aIndexData;


		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateBuffer(myIndexBufferDesc, myInitData,
			&myIndexBuffer->myIndexBuffer);
		if (FAILED(hr) != S_OK)
		{
			DL_ASSERT("BaseModel::SetupIndexBuffer: Failed to SetupIndexBuffer");
		}

		Engine::GetInstance()->SetDebugName(myIndexBuffer->myIndexBuffer, aDebugName);
	}

	void BaseModel::OnEffectLoad()
	{
		for (int i = 0; i < mySurfaces.Size(); ++i)
		{
			mySurfaces[i]->ReloadSurface();
		}
	}
}