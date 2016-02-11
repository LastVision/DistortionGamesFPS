#include "stdafx.h"
#include "TransformationNode.h"


namespace Prism
{
	TransformationNode::TransformationNode()
	{
	}

	TransformationNode::TransformationNode(const CU::Matrix44<float>& aOrientation)
		: myOrientation(aOrientation)
	{
	}

	const CU::Matrix44<float> TransformationNode::GetTransformationForTime(float aTime)
	{
		aTime;
		return myOrientation;
	}

	void TransformationNode::SetOrientation(const CU::Matrix44<float>& aOrientation)
	{
		myOrientation = aOrientation;
	}
}