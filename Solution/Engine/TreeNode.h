#pragma once

#include <GrowingArray.h>
#include <Vector.h>

namespace CU
{
	namespace Intersection
	{
		struct AABB;
	}
}



namespace Prism
{
	class Frustum;
	class Instance;

	class TreeNode
	{
	public:
		TreeNode(const CU::Vector3<float>& aPosition, float aHalfWidth, TreeNode* aParent, int aDepth, int aMaxDepth);
		~TreeNode();

		void Update();

		void InsertObjectDown(Instance* anObject);
		void Remove(Instance* anObject);
		void GetOccupantsInAABB(const Frustum& aFrustum
			, CU::GrowingArray<Instance*>& aOutArray);
	private:
		enum class eModifyType
		{
			INSERT,
			REMOVE,
		};
		void operator=(TreeNode&) = delete;

		void Modify(Instance* anObject, eModifyType aModifyType);
		CU::Vector3<float> GetMinCorner() const;
		CU::Vector3<float> GetMaxCorner() const;
		TreeNode* SpawnChild(int anId);
		bool NodeVsAABB(const CU::Intersection::AABB& aAABB) const;
		bool CheckEnclosed(Instance* anObject) const;
		void InsertObjectUp(Instance* anObject);

		const CU::Vector3<float> myPosition;
		const float myHalfWidth;
		TreeNode* const myParent;
		const int myDepth;
		const int myMaxDepth;

		float myLooseness;
		float myLooseWidth;
		TreeNode* myChildren[8];
		CU::GrowingArray<Instance*> myObjectsDynamic;
		CU::GrowingArray<Instance*> myObjectsStatic;
		bool myContainsObject;
	};


	inline void Prism::TreeNode::InsertObjectDown(Instance* anObject)
	{
		Modify(anObject, eModifyType::INSERT);
	}

	inline void Prism::TreeNode::Remove(Instance* anObject)
	{
		Modify(anObject, eModifyType::REMOVE);
	}

	inline CU::Vector3<float> TreeNode::GetMinCorner() const
	{
		return myPosition - myHalfWidth;
	}

	inline CU::Vector3<float> TreeNode::GetMaxCorner() const
	{
		return myPosition + myHalfWidth;
	}
}