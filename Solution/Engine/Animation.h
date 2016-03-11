#pragma once

#include <Defines.h>
#include "HierarchyBone.h"
#include <StaticArray.h>

namespace Prism
{	
	class AnimationNode;

	class Animation
	{
	public:
		Animation();
		~Animation();

		void Update(float aCurrentTime, CU::StaticArray<CU::Matrix44<float>, MAX_NR_OF_BONES>& aApplyArray);

		void AddAnimation(AnimationNode* aNode);
		void SetBoneMatrix(const CU::Matrix44<float>& aMatrix, int aIndex);
		void SetBoneBindPose(const CU::Matrix44<float>& aMatrix, int aIndex);
		void SetBindMatrix(const CU::Matrix44<float>& aMatrix);
		void AddBoneName(const std::string& aName);
		void SetHierarchy(const HierarchyBone& aBone);

		float GetAnimationLenght() const;
		void SetAnimationLenght(float aLenght);

		void GetHiearchyToBone(const std::string& aBoneName, GUIBone& aBoneOut);

	private:
		void SetBoneData(HierarchyBone& aBone);

		float myAnimationLenght;
		HierarchyBone myHierarchy;

		CU::GrowingArray<std::string> myBoneNames;
		CU::Matrix44<float> myBindMatrix;
		CU::StaticArray<CU::Matrix44<float>, MAX_NR_OF_BONES> myBones;
		CU::StaticArray<CU::Matrix44<float>, MAX_NR_OF_BONES> myBindPoses;
		CU::StaticArray<CU::Matrix44<float>, MAX_NR_OF_BONES> myCalculatedBones;
		CU::GrowingArray<AnimationNode*> myAnimationData;
	};

	inline float Animation::GetAnimationLenght() const
	{
		return myAnimationLenght;
	}

	inline void Animation::SetAnimationLenght(float aLenght)
	{
		myAnimationLenght = aLenght;
	}
}