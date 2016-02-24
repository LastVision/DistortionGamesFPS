#include "stdafx.h"

#include "AnimationNode.h"
#include "HierarchyBone.h"


namespace Prism
{
	HierarchyBone::HierarchyBone()
		: myCurrentAnimation(nullptr)
		, myResultMatrix(nullptr)
		, myBoneMatrix(nullptr)
		, myBoneID(0)
		, myParent(nullptr)
	{
		//We cannot init myChildren here, because stackoverflow
		//will get inited by BuildBoneHierarchy in FBXFactory
	}


	HierarchyBone::~HierarchyBone()
	{
	}

	void HierarchyBone::Update(const CU::Matrix44<float>& aParentMatrix, float aCurrentTime)
	{
		CU::Matrix44<float> calculatedMatrix = aParentMatrix;

		if (myResultMatrix)
		{
			if (myCurrentAnimation != nullptr && myCurrentAnimation->HasValues()
				&& myCurrentAnimation->GetAnimationLenght() > 0.f)
			{
				calculatedMatrix = myCurrentAnimation->GetCurrentMatrix(aCurrentTime) * aParentMatrix;
			}
			else
			{
				calculatedMatrix = (*myBoneMatrix) * aParentMatrix;
			}

			(*myResultMatrix) = calculatedMatrix;
		}

		for (int i = 0; i < myChildren.Size(); ++i)
		{
			myChildren[i].Update(calculatedMatrix, aCurrentTime);
		}
	}

	void HierarchyBone::GetBoneMatrix(const std::string& aBoneName, CU::Matrix44<float>& aOutMatrix) const
	{
		if (myBoneName == aBoneName)
		{
			aOutMatrix *= *myResultMatrix;
			return;
		}

		for (int i = 0; i < myChildren.Size(); ++i)
		{
			myChildren[i].GetBoneMatrix(aBoneName, aOutMatrix);
		}
	}

	bool HierarchyBone::GetBoneHiearchyByName(const std::string& aName, GUIBone& aOutBone)
	{
		if (myBoneName.compare(aName) == 0)
		{
			//aOutMatrixes.Add(BoneName(myBoneMatrix, myBoneName, myBoneID));
			aOutBone.myJoint = myBoneMatrix;
			aOutBone.myJointID = myBoneID;
			return true;
		}
		for (HierarchyBone &child : myChildren)
		{
			if (child.GetBoneHiearchyByName(aName, aOutBone))
			{
				//aOutMatrixes.Add(BoneName(myBoneMatrix, myBoneName, myBoneID));
				return true;
			}
		}
		return false;
	}
}