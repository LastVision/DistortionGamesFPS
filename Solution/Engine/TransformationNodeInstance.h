#pragma once

#include <GrowingArray.h>
#include <Matrix44.h>

namespace Prism
{
	class TransformationNode;

	class TransformationNodeInstance
	{
	public:
		TransformationNodeInstance();
		~TransformationNodeInstance();

		TransformationNodeInstance& operator=(const TransformationNodeInstance& aTransformationNode);

		void AddChildNode(TransformationNodeInstance* aChildNode);
		CU::GrowingArray<TransformationNodeInstance*>& GetChildren();
		const CU::Matrix44<float> GetTransformation();
		TransformationNode* GetTransformationNode();

		void SetTransformationNode(TransformationNode* aTransformationNode);

		void Update(float aElapsedTime, float aAnimationLenght);

	private:
		CU::GrowingArray<TransformationNodeInstance*> myChildren;
		float myElapsedTime;
		TransformationNode* myTransformationNode;
	};
}