#pragma once
#include "BoneName.h"

namespace Prism
{
	class AnimationNode;

	class HierarchyBone
	{
		friend class Animation;
		friend class FBXFactory;
		friend class DGFXLoader;
	public:
		HierarchyBone();
		~HierarchyBone();

		void Update(const CU::Matrix44<float>& aParentMatrix, float aCurrentTime);
		void GetBoneMatrix(const std::string& aBoneName, CU::Matrix44<float>& aOutMatrix) const;
		bool GetBoneHiearchyByName(const std::string& aName, GUIBone& aOutBone);

	private:
		HierarchyBone* myParent;
		CU::Matrix44<float>* myResultMatrix;
		CU::Matrix44<float>* myBoneMatrix;
		AnimationNode* myCurrentAnimation;
		CU::GrowingArray<HierarchyBone> myChildren;
		int myBoneID;
		std::string myBoneName;
	};
}