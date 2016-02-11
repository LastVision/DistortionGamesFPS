#include "stdafx.h"

#include "Animation.h"
#include <CommonHelper.h>
#include <d3dx11effect.h>
#include "Effect.h"
#include "EffectContainer.h"
#include "Engine.h"
#include "IndexBufferWrapper.h"
#include "ModelAnimated.h"
#include "Surface.h"
#include "VertexBufferWrapper.h"
#include "VertexDataWrapper.h"
#include "VertexIndexWrapper.h"
#include "TransformationNode.h"

namespace Prism
{
	ModelAnimated::ModelAnimated()
		: BaseModel()
		, myChildren(32)
		, myChildTransforms(32)
		, myIsNULLObject(true)
		, myVertexBaseData(nullptr)
		, myIndexBaseData(nullptr)
		, myInited(false)
		, myParent(nullptr)
		, myVertexCount(0)
		, myTransformation(nullptr)
		, myAnimation(nullptr)
	{
	}

	ModelAnimated::~ModelAnimated()
	{
		myChildren.DeleteAll();
		myChildTransforms.DeleteAll();
		delete myAnimation;
		delete myVertexBaseData;
		delete myIndexBaseData;
	}

	void ModelAnimated::Init()
	{
		DL_ASSERT_EXP(myInited == false, "Tried to Init a ModelAnimated twice");

		if (myIsNULLObject == false)
		{
			const int size = myVertexFormat.Size();
			D3D11_INPUT_ELEMENT_DESC* vertexDesc = new D3D11_INPUT_ELEMENT_DESC[size];
			for (int i = 0; i < myVertexFormat.Size(); ++i)
			{
				vertexDesc[i] = *myVertexFormat[i];
			}
			EvaluateEffectTechnique(false);
			InitInputLayout(vertexDesc, size, "ModelAnimated::InputLayout");
			delete[] vertexDesc;
			InitVertexBuffer(myVertexBaseData->myStride, D3D11_USAGE_IMMUTABLE, 0);
			InitIndexBuffer();

			SetupVertexBuffer(myVertexBaseData->myNumberOfVertices
				, myVertexBaseData->myStride, myVertexBaseData->myVertexData, "ModelAnimated::VertexBuffer");

			SetupIndexBuffer(myIndexBaseData->myNumberOfIndices, myIndexBaseData->myIndexData, "ModelAnimated::IndexBuffer");

			myVertexCount = myVertexBaseData->myNumberOfVertices;
		}

		for (int i = 0; i < myChildren.Size(); ++i)
		{
			myChildren[i]->myFileName = myFileName;
			myChildren[i]->Init();
		}


		myInited = true;

		if (myEffect->GetFileName() == "Data/Resource/Shader/S_effect_pbl.fx")
		{
			for (int i = 0; i < mySurfaces.Size(); ++i)
			{
				if (mySurfaces[i]->VerifyTextures(myFileName) == false)
				{
					DL_ASSERT(CU::Concatenate("Missing PBL-texture from ModelAnimated: %s", myFileName.c_str()));
				}
			}
		}
	}

	void ModelAnimated::AddChild(ModelAnimated* aChild)
	{
		aChild->myParent = this;
		myChildren.Add(aChild);
		myChildTransforms.Add(new TransformationNode(aChild->myOrientation));
	}

	void ModelAnimated::SetEffect(Effect* aEffect)
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

	void ModelAnimated::Render(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aCameraPosition, bool aIsDepthRender)
	{
		aCameraPosition;
		if (myIsNULLObject == false)
		{
			float blendFactor[4];
			blendFactor[0] = 0.f;
			blendFactor[1] = 0.f;
			blendFactor[2] = 0.f;
			blendFactor[3] = 0.f;

			myEffect->SetBlendState(NULL, blendFactor);
			myEffect->SetWorldMatrix(aOrientation);

			BaseModel::Render(aIsDepthRender);
		}
	}

	void ModelAnimated::ActivateAlbedo(eOwnerType aOwner)
	{
		for (int i = 0; i < mySurfaces.Size(); ++i)
		{
			mySurfaces[i]->ActivateAlbedo(aOwner);
		}

		for (int i = 0; i < myChildren.Size(); ++i)
		{
			myChildren[i]->ActivateAlbedo(aOwner);
		}
	}
}