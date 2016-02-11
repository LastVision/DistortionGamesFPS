#include "stdafx.h"

#include "TransformationNode.h"
#include "TransformationNodeInstance.h"


namespace Prism
{
	TransformationNodeInstance::TransformationNodeInstance()
		: myTransformationNode(nullptr)
		, myChildren(4)
		, myElapsedTime(0.f)
	{
	}


	TransformationNodeInstance::~TransformationNodeInstance()
	{
		//SAFE_DELETE(myTransformationNode);
		//if (myChildren.Size() > 0)
		//{
		//	myChildren.DeleteAll();
		//}
	}

	TransformationNodeInstance& TransformationNodeInstance::operator=(const TransformationNodeInstance& aTransformationNode)
	{
		myElapsedTime = aTransformationNode.myElapsedTime;
		myTransformationNode = aTransformationNode.myTransformationNode;
		for (int i = 0; i < aTransformationNode.myChildren.Size(); ++i)
		{
			myChildren.Add(aTransformationNode.myChildren[i]);
		}

		return *this;
	}

	void TransformationNodeInstance::AddChildNode(TransformationNodeInstance* aChildNode)
	{
		myChildren.Add(aChildNode);
	}

	CU::GrowingArray<TransformationNodeInstance*>& TransformationNodeInstance::GetChildren()
	{
		return myChildren;
	}

	const CU::Matrix44<float> TransformationNodeInstance::GetTransformation()
	{
		if (myTransformationNode == nullptr)
		{
			return CU::Matrix44<float>();
		}
		return myTransformationNode->GetTransformationForTime(myElapsedTime);
	}

	TransformationNode* TransformationNodeInstance::GetTransformationNode()
	{
		return myTransformationNode;
	}


	void TransformationNodeInstance::SetTransformationNode(TransformationNode* aTransformationNode)
	{
		myTransformationNode = aTransformationNode;
	}

	void TransformationNodeInstance::Update(float aElapsedTime, float aAnimationLenght)
	{
		myElapsedTime += aElapsedTime;
		for (int i = 0; i < myChildren.Size(); ++i)
		{
			myChildren[i]->Update(aElapsedTime, aAnimationLenght);
		}

		if (myElapsedTime > aAnimationLenght)
		{
			myElapsedTime = 0.f;
		}
	}
}