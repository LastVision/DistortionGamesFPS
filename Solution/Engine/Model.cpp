#include "stdafx.h"

#include <CommonHelper.h>
#include <d3dx11effect.h>
#include "Effect.h"
#include "EffectContainer.h"
#include "Engine.h"
#include "IndexBufferWrapper.h"
#include "Model.h"
#include "Surface.h"
#include "VertexBufferWrapper.h"
#include "VertexDataWrapper.h"
#include "VertexIndexWrapper.h"

namespace Prism
{

	Model::Model()
		: BaseModel()
		, myChildren(32)
		, myChildTransforms(32)
		, myIsNULLObject(true)
		, myVertexBaseData(nullptr)
		, myIndexBaseData(nullptr)
		, myIsLodGroup(false)
		, myLodGroup(nullptr)
		, myInited(false)
		, myParent(nullptr)
		, myVertexCount(0)
		, myMaxInstances(0)
		, myInstancingMatrixBuffer(nullptr)
		, myInstancingScaleBuffer(nullptr)
		, myInstancingHeightBuffer(nullptr)
		, myMatrices(128)
		, myScales(128)
		, myHeights(128)
	{
		myInstancingBufferDesc = new D3D11_BUFFER_DESC();
	}

	Model::~Model()
	{
		myChildren.DeleteAll();
		delete myVertexBaseData;
		delete myIndexBaseData;
		SAFE_DELETE(myInstancingBufferDesc);
		
		if (myInstancingMatrixBuffer != nullptr && myInstancingMatrixBuffer->myVertexBuffer != nullptr)
		{
			myInstancingMatrixBuffer->myVertexBuffer->Release();
			delete myInstancingMatrixBuffer;
		}

		if (myInstancingScaleBuffer != nullptr && myInstancingScaleBuffer->myVertexBuffer != nullptr)
		{
			myInstancingScaleBuffer->myVertexBuffer->Release();
			delete myInstancingScaleBuffer;
		}

		if (myInstancingHeightBuffer != nullptr && myInstancingHeightBuffer->myVertexBuffer != nullptr)
		{
			myInstancingHeightBuffer->myVertexBuffer->Release();
			delete myInstancingHeightBuffer;
		}
	}

	void Model::Init(int aMaxInstances, bool aLightMesh)
	{
		DL_ASSERT_EXP(myInited == false, "Tried to Init a model twice");

		myMaxInstances = aMaxInstances;
		if (myIsNULLObject == false)
		{
			const int size = myVertexFormat.Size() + 6;
			D3D11_INPUT_ELEMENT_DESC* vertexDesc = new D3D11_INPUT_ELEMENT_DESC[size];
			for (int i = 0; i < myVertexFormat.Size(); ++i)
			{
				vertexDesc[i] = *myVertexFormat[i];
			}

			if (aLightMesh == false)
			{
				vertexDesc[myVertexFormat.Size() + 0] = { "myWorld", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
				vertexDesc[myVertexFormat.Size() + 1] = { "myWorld", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
				vertexDesc[myVertexFormat.Size() + 2] = { "myWorld", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
				vertexDesc[myVertexFormat.Size() + 3] = { "myWorld", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 };

				vertexDesc[myVertexFormat.Size() + 4] = { "myScale", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
				vertexDesc[myVertexFormat.Size() + 5] = { "myHeight", 0, DXGI_FORMAT_R32_FLOAT, 3, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
			}

			
			if (aLightMesh == false)
			{
				EvaluateEffectTechnique(true);
				InitInputLayout(vertexDesc, size, "Model::InputLayout");
			}
			else
			{
				//EvaluateEffectTechnique(false);
				myTechniqueName = "Render";
				InitInputLayout(vertexDesc, size-6, "Model::InputLayout");
			}
			delete[] vertexDesc;
			InitVertexBuffer(myVertexBaseData->myStride, D3D11_USAGE_IMMUTABLE, 0);
			InitIndexBuffer();
			InitInstancingBuffers();

			SetupVertexBuffer(myVertexBaseData->myNumberOfVertices
				, myVertexBaseData->myStride, myVertexBaseData->myVertexData, "Model::VertexBuffer");

			SetupIndexBuffer(myIndexBaseData->myNumberOfIndices, myIndexBaseData->myIndexData, "Model::IndexBuffer");
			SetupInstancingBuffers();
			myVertexCount = myVertexBaseData->myNumberOfVertices;
		}

		for (int i = 0; i < myChildren.Size(); ++i)
		{
			myChildren[i]->myFileName = myFileName;
			myChildren[i]->Init(myMaxInstances, aLightMesh);
		}


		myInited = true;

		if (myEffect->GetFileName() == "Data/Resource/Shader/S_effect_pbl.fx")
		{
			for (int i = 0; i < mySurfaces.Size(); ++i)
			{
				if (mySurfaces[i]->VerifyTextures(myFileName) == false)
				{
					DL_ASSERT(CU::Concatenate("Missing PBL-texture from model: %s", myFileName.c_str()));
				}
			}
		}
	}

	void Model::InitCube(const float aWidth, const float aHeight, const float aDepth, CU::Vector4f aColour)
	{
		DL_ASSERT_EXP(myInited == false, "Tried to Init a model twice");
		myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_cube3d.fx");


		if (myEffect == nullptr)
		{
			DL_MESSAGE_BOX("Failed to GetEffect", "InitLightCube", MB_ICONWARNING);
		}

		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "Model::InputLayout");


#pragma region Vertices
		CU::GrowingArray<VertexPosColor> vertices;
		vertices.Init(24);

		float halfWidth = aWidth / 2.f;
		float halfHeight = aHeight / 2.f;
		float halfDepth = aDepth / 2.f;

		//0 - 3 (Top)
		vertices.Add({ { -halfWidth, halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, halfHeight, halfDepth, 0.f }, aColour });
		vertices.Add({ { -halfWidth, halfHeight, halfDepth, 0.f }, aColour });

		//4 - 7 (Bottom)
		vertices.Add({ { -halfWidth, -halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, -halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, -halfHeight, halfDepth, 0.f }, aColour });
		vertices.Add({ { -halfWidth, -halfHeight, halfDepth, 0.f }, aColour });

		//8 - 11 (Left)
		vertices.Add({ { -halfWidth, -halfHeight, halfDepth, 0.f }, aColour });
		vertices.Add({ { -halfWidth, -halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { -halfWidth, halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { -halfWidth, halfHeight, halfDepth, 0.f }, aColour });

		//12 - 15 (Right)
		vertices.Add({ { halfWidth, -halfHeight, halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, -halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, halfHeight, halfDepth, 0.f }, aColour });

		//16 - 19 (Front)
		vertices.Add({ { -halfWidth, -halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, -halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, halfHeight, -halfDepth, 0.f }, aColour });
		vertices.Add({ { -halfWidth, halfHeight, -halfDepth, 0.f }, aColour });

		//20 - 23 (Back)
		vertices.Add({ { -halfWidth, -halfHeight, halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, -halfHeight, halfDepth, 0.f }, aColour });
		vertices.Add({ { halfWidth, halfHeight, halfDepth, 0.f }, aColour });
		vertices.Add({ { -halfWidth, halfHeight, halfDepth, 0.f }, aColour });
#pragma endregion

#pragma region Indices

		CU::GrowingArray<int> indices(24);
		//Top
		indices.Add(3);
		indices.Add(1);
		indices.Add(0);

		indices.Add(2);
		indices.Add(1);
		indices.Add(3);

		//Bottom
		indices.Add(6);
		indices.Add(4);
		indices.Add(5);

		indices.Add(7);
		indices.Add(4);
		indices.Add(6);

		//Left
		indices.Add(11);
		indices.Add(9);
		indices.Add(8);

		indices.Add(10);
		indices.Add(9);
		indices.Add(11);

		//Right
		indices.Add(14);
		indices.Add(12);
		indices.Add(13);

		indices.Add(15);
		indices.Add(12);
		indices.Add(14);

		//Front
		indices.Add(19);
		indices.Add(17);
		indices.Add(16);

		indices.Add(18);
		indices.Add(17);
		indices.Add(19);

		//Back
		indices.Add(22);
		indices.Add(20);
		indices.Add(21);

		indices.Add(23);
		indices.Add(20);
		indices.Add(22);

#pragma endregion


		InitVertexBuffer(sizeof(VertexPosColor), D3D11_USAGE_IMMUTABLE, 0);
		InitIndexBuffer();

		SetupVertexBuffer(vertices.Size(), sizeof(VertexPosColor), reinterpret_cast<char*>(&vertices[0]), "Model::VertexBuffer");
		SetupIndexBuffer(indices.Size(), reinterpret_cast<char*>(&indices[0]), "Model::IndexBuffer");

		Surface surf;
		surf.SetEffect(myEffect);
		surf.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		surf.SetVertexStart(0);
		surf.SetVertexCount(vertices.Size());
		surf.SetIndexStart(0);
		surf.SetIndexCount(indices.Size());
		surf.SetTexture("DiffuseTexture", "Data/resources/texture/T_missing_texture.dds", true);

		mySurfaces.Add(new Surface(surf));

		myIsNULLObject = false;
		myInited = true;
	}

	void Model::AddChild(Model* aChild)
	{
		aChild->myParent = this;
		myChildren.Add(aChild);
		myChildTransforms.Add(aChild->myOrientation);
	}

	void Prism::Model::SetLodGroup(LodGroup* aLodGroup)
	{
		myLodGroup = aLodGroup;
		if (myLodGroup != nullptr)
		{
			myIsLodGroup = true;
		}
	}

	void Model::SetEffect(Effect* aEffect)
	{
		myEffect = aEffect;
		myEffect->AddListener(this);

		for (int i = 0; i < mySurfaces.Size(); ++i)
		{
			mySurfaces[i]->SetEffect(myEffect);
			mySurfaces[i]->ReloadSurface();
		}

		for (int i = 0; i < myChildren.Size(); ++i)
		{
			myChildren[i]->SetEffect(aEffect);
		}
	}

	void Model::Render(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aCameraPosition)
	{
		if (myIsLodGroup == true)
		{
			float lengthBetweenCameraAndModel = CU::Length(aCameraPosition - aOrientation.GetPos());
			int level = 0;

			Model* toRender = nullptr;
			for (int i = myChildren.Size() - 1; i >= 0; i--)
			{
				LodGroup* group = myLodGroup;
				double threshold = group->myThreshHolds[i];
				threshold /= 100;
				if (threshold <= lengthBetweenCameraAndModel)
				{
					toRender = myChildren[i];
					level = i;
					break;
				}
			}

			if (toRender)
			{
				toRender->Render(aOrientation, aCameraPosition);
			}
		}
		else
		{
			if (myIsNULLObject == false)
			{
				float blendFactor[4];
				blendFactor[0] = 0.f;
				blendFactor[1] = 0.f;
				blendFactor[2] = 0.f;
				blendFactor[3] = 0.f;

				myEffect->SetBlendState(NULL, blendFactor);
				myEffect->SetWorldMatrix(aOrientation);

				BaseModel::Render();
			}

			for (int i = 0; i < myChildren.Size(); ++i)
			{
				myChildren[i]->Render(myChildTransforms[i] * aOrientation, aCameraPosition);
			}
		}
	}

	void Model::RenderOcculus(const CU::Matrix44<float>& aOrientation, const CU::Matrix44<float>& aViewProjection)
	{
		//float blendFactor[4];
		//blendFactor[0] = 0.f;
		//blendFactor[1] = 0.f;
		//blendFactor[2] = 0.f;
		//blendFactor[3] = 0.f;

		//myEffect->SetBlendState(NULL, blendFactor);
		myEffect->SetWorldMatrix(aOrientation);
		myEffect->SetViewProjectionMatrix(aViewProjection);

		BaseModel::Render();
	}

	void Model::DeActivateSurfaces()
	{
		for (int i = 0; i < mySurfaces.Size(); ++i)
		{
			mySurfaces[i]->DeActivate();
		}
	}

	bool Model::SetGPUState(const CU::GrowingArray<CU::Matrix44<float>>& someWorldMatrices
		, const CU::GrowingArray<CU::Vector3<float>>& someScales
		, const CU::GrowingArray<float>& someHeights)
	{
		DL_ASSERT_EXP(mySurfaces.Size() < 2, "We do not support several surfaces yet");

		if (myIsNULLObject == true)
		{
			//if (myChildren.Size() == 0)
			{
				return false;
			}
			//return myChildren[0]->SetGPUState(someWorldMatrices, someScales, someHeights);
		}
		else
		{
			DL_ASSERT_EXP(someWorldMatrices.Size() <= myMaxInstances, "Tried to instance to many instances");

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();

			context->Map(myInstancingMatrixBuffer->myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (mappedResource.pData != nullptr)
			{
				CU::Matrix44<float>* data = (CU::Matrix44<float>*)mappedResource.pData;
				if (someWorldMatrices.Size() > 0)
				{
					memcpy(data, &someWorldMatrices[0], sizeof(CU::Matrix44<float>) * someWorldMatrices.Size());
				}

				context->Unmap(myInstancingMatrixBuffer->myVertexBuffer, 0);
			}
			else
			{
				DL_ASSERT("Failed to Map InstancingMatrix Buffer");
				return false;
			}

			context->Map(myInstancingScaleBuffer->myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (mappedResource.pData != nullptr)
			{
				CU::Vector3<float>* data = (CU::Vector3<float>*)mappedResource.pData;
				if (someScales.Size() > 0)
				{
					memcpy(data, &someScales[0], sizeof(CU::Vector3<float>) * someScales.Size());
				}

				context->Unmap(myInstancingScaleBuffer->myVertexBuffer, 0);
			}
			else
			{
				DL_ASSERT("Failed to Map InstancingScale Buffer");
				return false;
			}

			context->Map(myInstancingHeightBuffer->myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (mappedResource.pData != nullptr)
			{
				float* data = (float*)mappedResource.pData;
				if (someHeights.Size() > 0)
				{
					memcpy(data, &someHeights[0], sizeof(float) * someHeights.Size());
				}

				context->Unmap(myInstancingHeightBuffer->myVertexBuffer, 0);
			}
			else
			{
				DL_ASSERT("Failed to Map InstancingHeight Buffer");
				return false;
			}

			myVertexBuffers[0] = myVertexBuffer->myVertexBuffer;
			myVertexBuffers[1] = myInstancingMatrixBuffer->myVertexBuffer;
			myVertexBuffers[2] = myInstancingScaleBuffer->myVertexBuffer;
			myVertexBuffers[3] = myInstancingHeightBuffer->myVertexBuffer;

			UINT strides[4] = { myVertexBuffer->myStride, myInstancingMatrixBuffer->myStride
				, myInstancingScaleBuffer->myStride, myInstancingHeightBuffer->myStride };
			UINT offsets[4] = { 0, 0, 0, 0 };
			context->IASetVertexBuffers(0, 4, myVertexBuffers, strides, offsets);
			context->IASetIndexBuffer(myIndexBuffer->myIndexBuffer
				, myIndexBuffer->myIndexBufferFormat, myIndexBuffer->myByteOffset);
			context->IASetInputLayout(myVertexLayout);

			mySurfaces[0]->Activate();

			return true;
		}
	}

	int Model::GetIndexCount()
	{
		if (myIsNULLObject == true)
		{
			return myChildren[0]->GetIndexCount();
		}
		else
		{
			return mySurfaces[0]->GetIndexCount();
		}
	}

	int Model::GetVertexStart()
	{
		if (myIsNULLObject == true)
		{
			return myChildren[0]->GetVertexStart();
		}
		else
		{
			return mySurfaces[0]->GetVertexStart();
		}
	}

	CU::GrowingArray<Model*>& Model::GetChildren()
	{
		return myChildren;
	}

	const std::string& Model::GetTechniqueName() const
	{
		if (myIsNULLObject == true)
		{
			return myChildren[0]->GetTechniqueName();
		}
		else
		{
			return BaseModel::GetTechniqueName();
		}
	}

	Model* Model::GetRealModel(const CU::Vector3<float>& aModelPosition, const CU::Vector3<float>& aCameraPosition)
	{
		if (myIsLodGroup == true)
		{
			float lengthBetweenCameraAndModel = CU::Length(aCameraPosition - aModelPosition);
			int level = 0;

			Model* toRender = nullptr;
			for (int i = myChildren.Size() - 1; i >= 0; i--)
			{
				LodGroup* group = myLodGroup;
				double threshold = group->myThreshHolds[i];
				threshold /= 100;
				if (threshold <= lengthBetweenCameraAndModel)
				{
					toRender = myChildren[i];
					level = i;
					break;
				}
			}

			if (toRender)
			{
				return toRender->GetRealModel(aModelPosition, aCameraPosition);
			}

		}
		return this;
	}

	void Model::InitInstancingBuffers()
	{
		ZeroMemory(myInstancingBufferDesc, sizeof(*myInstancingBufferDesc));
		myInstancingBufferDesc->Usage = D3D11_USAGE_DYNAMIC;
		myInstancingBufferDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;
		myInstancingBufferDesc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		myInstancingBufferDesc->MiscFlags = 0;
		myInstancingBufferDesc->StructureByteStride = 0;


		myInstancingMatrixBuffer = new VertexBufferWrapper();
		myInstancingMatrixBuffer->myStride = sizeof(CU::Matrix44<float>);
		myInstancingMatrixBuffer->myByteOffset = 0;
		myInstancingMatrixBuffer->myStartSlot = 0;
		myInstancingMatrixBuffer->myNumberOfBuffers = 1;

		myInstancingScaleBuffer = new VertexBufferWrapper();
		myInstancingScaleBuffer->myStride = sizeof(CU::Vector3<float>);
		myInstancingScaleBuffer->myByteOffset = 0;
		myInstancingScaleBuffer->myStartSlot = 0;
		myInstancingScaleBuffer->myNumberOfBuffers = 1;

		myInstancingHeightBuffer = new VertexBufferWrapper();
		myInstancingHeightBuffer->myStride = sizeof(float);
		myInstancingHeightBuffer->myByteOffset = 0;
		myInstancingHeightBuffer->myStartSlot = 0;
		myInstancingHeightBuffer->myNumberOfBuffers = 1;
	}

	void Model::SetupInstancingBuffers()
	{
		if (myInstancingMatrixBuffer->myVertexBuffer != nullptr)
			myInstancingMatrixBuffer->myVertexBuffer->Release();

		myInstancingBufferDesc->ByteWidth = sizeof(CU::Matrix44<float>) * myMaxInstances;
		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateBuffer(myInstancingBufferDesc, nullptr
			, &myInstancingMatrixBuffer->myVertexBuffer);
		if (FAILED(hr) != S_OK)
		{
			DL_ASSERT("Model::SetupInstancingBuffer: Failed to setup myInstancingMatrixBuffer");
		}
		Engine::GetInstance()->SetDebugName(myInstancingMatrixBuffer->myVertexBuffer, "Model::myInstancingMatrixBuffer->myVertexBuffer");


		if (myInstancingScaleBuffer->myVertexBuffer != nullptr)
			myInstancingScaleBuffer->myVertexBuffer->Release();

		myInstancingBufferDesc->ByteWidth = sizeof(CU::Vector3<float>) * myMaxInstances;
		hr = Engine::GetInstance()->GetDevice()->CreateBuffer(myInstancingBufferDesc, nullptr
			, &myInstancingScaleBuffer->myVertexBuffer);
		if (FAILED(hr) != S_OK)
		{
			DL_ASSERT("Model::SetupInstancingBuffer: Failed to setup myInstancingScaleBuffer");
		}
		Engine::GetInstance()->SetDebugName(myInstancingScaleBuffer->myVertexBuffer, "Model::myInstancingScaleBuffer->myVertexBuffer");

		if (myInstancingHeightBuffer->myVertexBuffer != nullptr)
			myInstancingHeightBuffer->myVertexBuffer->Release();

		myInstancingBufferDesc->ByteWidth = sizeof(float) * myMaxInstances;
		hr = Engine::GetInstance()->GetDevice()->CreateBuffer(myInstancingBufferDesc, nullptr
			, &myInstancingHeightBuffer->myVertexBuffer);
		if (FAILED(hr) != S_OK)
		{
			DL_ASSERT("Model::SetupInstancingBuffer: Failed to setup myInstancingHeightBuffer");
		}
		Engine::GetInstance()->SetDebugName(myInstancingHeightBuffer->myVertexBuffer, "Model::myInstancingHeightBuffer->myVertexBuffer");
	}

	bool Model::IsNullObject()
	{
		return myIsNULLObject;
	}

}