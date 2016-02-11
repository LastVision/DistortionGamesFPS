#pragma once

#include "BaseModel.h"
#include "EffectListener.h"
#include <GrowingArray.h>
#include <Matrix.h>
#include "Vertices.h"
#include "LodStructs.h"


namespace Prism
{
	class Effect;
	class Surface;

	struct IndexBufferWrapper;
	struct MeshData;
	struct VertexBufferWrapper;
	struct VertexDataWrapper;
	struct VertexIndexWrapper;

	
	class Model : public BaseModel
	{
		friend class DGFXLoader;
		friend class FBXFactory;
		friend class ModelLoader;
	public:
		Model();
		~Model();

		void Init(int aMaxInstances);
		void InitCube(float aWidth = 1.f, float aHeight = 1.f, float aDepth = 1.f
				, CU::Vector4f aColour = { 0.7f, 0.7f, 0.7f, 1.f });

		void AddChild(Model* aChild);
		void SetLodGroup(LodGroup* aLodGroup);

		
		void SetEffect(Effect* aEffect);

		void Render(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aCameraPosition, bool aIsDepthRender);

		void DeActivateSurfaces();
		void ActivateAlbedo(eOwnerType aOwner);
		
		bool SetGPUState(const CU::GrowingArray<CU::Matrix44<float>>& someWorldMatrices
			, const CU::GrowingArray<CU::Vector3<float>>& someScales
			, const CU::GrowingArray<float>& someHeights
			, eOwnerType aOwner);
		int GetIndexCount();
		int GetVertexStart();

		const std::string& GetTechniqueName() const override;
		Model* GetRealModel(const CU::Vector3<float>& aModelPosition, const CU::Vector3<float>& aCameraPosition);
		void SetFileName(const std::string& aFileName) override;

	private:
		void operator=(Model&) = delete;
		void InitInstancingBuffers();
		void SetupInstancingBuffers();

		bool myIsNULLObject;

		
		VertexIndexWrapper* myIndexBaseData;
		VertexDataWrapper* myVertexBaseData;

		CU::GrowingArray<Model*> myChildren;
		CU::GrowingArray<CU::Matrix44f> myChildTransforms;
		CU::Matrix44f myOrientation;

		int myVertexCount;
		Model* myParent;
		bool myInited;
		bool myIsLodGroup;
		LodGroup* myLodGroup;

		D3D11_BUFFER_DESC* myInstancingBufferDesc;
		VertexBufferWrapper* myInstancingMatrixBuffer;
		VertexBufferWrapper* myInstancingScaleBuffer;
		VertexBufferWrapper* myInstancingHeightBuffer;
		ID3D11Buffer* myVertexBuffers[4];
		int myMaxInstances;
	};

	inline void Model::SetFileName(const std::string& aFileName)
	{
		myFileName = aFileName;
		for (int i = 0; i < myChildren.Size(); ++i)
		{
			myChildren[i]->SetFileName(aFileName);
		}
	}
}