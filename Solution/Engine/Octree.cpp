#include "stdafx.h"

#include "Defines.h"
#include "EngineEnums.h"
#include "Instance.h"
#include "Octree.h"
#include <sstream>
#include "TreeNode.h"
#include <Vector.h>

Prism::Octree::Octree(int aMaxDepth)
	: myMaxDepth(aMaxDepth)
{
	myRoot = new TreeNode(CU::Vector3<float>(0, 0, 0), OCTREE_WIDTH, nullptr, 0, aMaxDepth);
}

Prism::Octree::~Octree()
{
	delete myRoot;
	myRoot = nullptr;
}

void Prism::Octree::Add(Instance* aInstance)
{
	DL_ASSERT_EXP(aInstance->GetOctreeType() != eOctreeType::NOT_IN_OCTREE, "Can't add NOT_IN_OCTREE object to octree.");
	myRoot->InsertObjectDown(aInstance);
}

void Prism::Octree::Remove(Instance* aInstance)
{
	myRoot->Remove(aInstance);
}
void Prism::Octree::Update()
{
	myRoot->Update();
}

void Prism::Octree::GetOccupantsInAABB(const Frustum& aFrustum
		, CU::GrowingArray<Instance*>& aOutArray)
{
	myRoot->GetOccupantsInAABB(aFrustum, aOutArray);
}