#pragma once

#include "BaseModel.h"
#include "EffectListener.h"
#include <GrowingArray.h>
#include <Matrix.h>
#include "Vertices.h"
#include "LodStructs.h"


namespace Prism
{
	class Animation;
	class Effect;
	class Surface;
	class TransformationNode;

	struct IndexBufferWrapper;
	struct MeshData;
	struct VertexBufferWrapper;
	struct VertexDataWrapper;
	struct VertexIndexWrapper;


	class ModelAnimated : public BaseModel
	{
		friend class FBXFactory;
		friend class DGFXLoader;
		friend class ModelLoader;
		friend class Instance;
	public:
		ModelAnimated();
		~ModelAnimated();

		void Init();

		void AddChild(ModelAnimated* aChild);

		void SetEffect(Effect* aEffect);

		void Render(const CU::Matrix44<float>& aOrientation);
		void SetFileName(const std::string& aFileName) override;

	private:
		bool myIsNULLObject;

		VertexIndexWrapper* myIndexBaseData;
		VertexDataWrapper* myVertexBaseData;

		CU::GrowingArray<ModelAnimated*> myChildren;
		CU::GrowingArray<TransformationNode*> myChildTransforms;
		TransformationNode* myTransformation;
		Animation* myAnimation;
		CU::Matrix44f myOrientation;

		int myVertexCount;
		ModelAnimated* myParent;
		bool myInited;
	};

	inline void ModelAnimated::SetFileName(const std::string& aFileName)
	{
		myFileName = aFileName;
		for (int i = 0; i < myChildren.Size(); ++i)
		{
			myChildren[i]->SetFileName(aFileName);
		}
	}
}