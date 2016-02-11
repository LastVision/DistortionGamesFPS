#pragma once
#include <GrowingArray.h>
#include "TransformationNode.h"

namespace Prism
{
	class TransformationNodeHierarchy : public TransformationNode
	{
	public:
		TransformationNodeHierarchy();
		~TransformationNodeHierarchy();

		void AddTransformation(TransformationNode* aNode);
		const CU::Matrix44<float> GetTransformationForTime(float aTime) override;

	private:
		CU::GrowingArray<TransformationNode*> myNodes;
	};
}