#include "stdafx.h"

#include "Animation.h"
#include "AnimationNode.h"
#include "HierarchyBone.h"

namespace Prism
{
	Animation::Animation()
		: myAnimationData(4)
		, myBoneNames(4)
		, myAnimationLenght(0.f)
	{
		for (int i = 0; i < MAX_NR_OF_BONES; i++)
		{
			myCalculatedBones[i] = CU::Matrix44<float>();
		}
	}


	Animation::~Animation()
	{
		myAnimationData.DeleteAll();
	}

	void Animation::Update(float aCurrentTime, CU::StaticArray<CU::Matrix44<float>, MAX_NR_OF_BONES>& aApplyArray)
	{
		if (aCurrentTime > myAnimationLenght)
		{
			aCurrentTime -= myAnimationLenght * floor(aCurrentTime / myAnimationLenght);
		}

		myHierarchy.Update(CU::Matrix44<float>(), aCurrentTime);

		for (int i = 0; i < myBoneNames.Size(); ++i)
		{
			myCalculatedBones[i] = myBindPoses[i] * myCalculatedBones[i];

			aApplyArray[i] = myCalculatedBones[i];
		}
	}

	void Animation::AddAnimation(AnimationNode* aNode)
	{
		myAnimationData.Add(aNode);
	}

	void Animation::SetBoneMatrix(const CU::Matrix44<float>& aMatrix, int aIndex)
	{
		DL_ASSERT_EXP(aIndex > -1, "aIndex is below 0");
		DL_ASSERT_EXP(aIndex < MAX_NR_OF_BONES, "aIndex is to large");

		myBones[aIndex] = aMatrix;
	}

	void Animation::SetBoneBindPose(const CU::Matrix44<float>& aMatrix, int aIndex)
	{
		DL_ASSERT_EXP(aIndex > -1, "aIndex is below 0");
		DL_ASSERT_EXP(aIndex < MAX_NR_OF_BONES, "aIndex is to large");

		myBindPoses[aIndex] = aMatrix;
	}

	void Animation::SetBindMatrix(const CU::Matrix44<float>& aMatrix)
	{
		myBindMatrix = aMatrix;
	}

	void Animation::AddBoneName(const std::string& aName)
	{
		myBoneNames.Add(aName);
	}

	void Animation::SetHierarchy(const HierarchyBone& aBone)
	{
		myHierarchy = aBone;
		SetBoneData(myHierarchy);
	}
	
	void Animation::GetHiearchyToBone(const std::string& aBoneName, GUIBone& aBoneOut)
	{
		myHierarchy.GetBoneHiearchyByName(aBoneName, aBoneOut);
		aBoneOut.myJoint = &myCalculatedBones[aBoneOut.myJointID];
		aBoneOut.myBind = &myBindPoses[aBoneOut.myJointID];
	}

	void Animation::SetBoneData(HierarchyBone& aBone)
	{
		if (aBone.myBoneID == -1)
		{
			for (int i = 0; i < myBoneNames.Size(); ++i)
			{
				if (myBoneNames[i] == aBone.myBoneName)
				{
					aBone.myBoneID = i;
				}
			}
		}

		for (int i = 0; i < myAnimationData.Size(); ++i)
		{
			if (myAnimationData[i]->myBoneName == aBone.myBoneName)
			{
				aBone.myCurrentAnimation = myAnimationData[i];
			}
		}

		if (aBone.myBoneID != -1)
		{
			aBone.myBoneMatrix = &myBones[aBone.myBoneID];
			aBone.myResultMatrix = &myCalculatedBones[aBone.myBoneID];
		}

		for (int i = 0; i < aBone.myChildren.Size(); ++i)
		{
			SetBoneData(aBone.myChildren[i]);
		}
	}
}