#include "stdafx.h"
#include "AnimationNode.h"


namespace Prism
{
	AnimationNode::AnimationNode(int aNumOfFrames)
		: myValues(max(1,aNumOfFrames))
		, myEndTime(0.f)
	{
	}


	AnimationNode::~AnimationNode()
	{
	}

	void AnimationNode::AddValue(const AnimationNodeValue& aFrame)
	{
		myValues.Add(aFrame);
		myEndTime = max(myEndTime, aFrame.myTime);
	}

	bool AnimationNode::HasValues() const
	{
		return myValues.Size() > 0;
	}

	void AnimationNode::CalculateEndTime()
	{
		for (int i = 0; i < myValues.Size(); ++i)
		{
			myEndTime = max(myEndTime, myValues[i].myTime);
		}
	}


	CU::Matrix44<float> AnimationNode::GetCurrentMatrix(float aTime)
	{
		CU::Matrix44<float>* startValue = &myValues[0].myMatrix;
		CU::Matrix44<float>* endValue = &myValues.GetLast().myMatrix;
		float alpha = 1.0f;
		if (aTime > myValues[0].myTime)
		{
			for (int i = 0; i < myValues.Size() - 1; i++)
			{
				const int currentId = i;
				float nextValue = myValues[currentId + 1].myTime;
				if (nextValue > aTime)
				{
					float currentValue = myValues[currentId].myTime;
					startValue = &myValues[currentId].myMatrix;
					endValue = &myValues[currentId + 1].myMatrix;
					alpha = (aTime - currentValue) / (nextValue - currentValue);
					break;
				}
			}
		}
		else
		{
			startValue = &myValues.GetLast().myMatrix;
			endValue = &myValues[0].myMatrix;
			if (myValues[0].myTime > 0 || myValues[0].myTime < 0)
			{
				alpha = (aTime) / (myValues[0].myTime);
			}

		}

		return (*startValue * (1.0f - alpha)) + (*endValue * alpha);
	}

	float AnimationNode::GetAnimationLenght()
	{
		return myEndTime;
	}
}