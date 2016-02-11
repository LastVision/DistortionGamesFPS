#include "stdafx.h"
#include "TransformationNodeHierarchy.h"


namespace Prism
{
	TransformationNodeHierarchy::TransformationNodeHierarchy()
		: myNodes(4)
	{
	}

	TransformationNodeHierarchy::~TransformationNodeHierarchy()
	{
	}

	void TransformationNodeHierarchy::AddTransformation(TransformationNode* aNode)
	{
		myNodes.Add(aNode);
	}

	const CU::Matrix44<float> TransformationNodeHierarchy::GetTransformationForTime(float aTime)
	{
		CU::Matrix44<float> matrix;
		for (int i = 0; i < myNodes.Size(); ++i)
		{
			matrix *= myNodes[i]->GetTransformationForTime(aTime);
		}

		return matrix;
	}
}