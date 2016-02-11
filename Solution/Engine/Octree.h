#pragma once

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
	class TreeNode;

	class Octree
	{
	public:
		Octree(int aMaxDepth);
		~Octree();

		void Add(Instance* aInstance);
		void Remove(Instance* aInstance);
		void Update();
		void GetOccupantsInAABB(const Frustum& aFrustum, CU::GrowingArray<Instance*>& aOutArray);


	private:
		void operator=(Octree&) = delete;

		const int myMaxDepth;
		TreeNode* myRoot;
	};
}