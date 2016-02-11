#include "stdafx.h"

#include "Defines.h"
#include "EngineEnums.h"
#include "Frustum.h"
#include "Instance.h"
#include <Intersection.h>
#include "TreeNode.h"

#ifdef SHOW_OCTREE_DEBUG
#include <sstream>
std::stringstream ss;
std::stringstream ss2;
std::stringstream ss3;
std::stringstream ss4;
std::stringstream ss5;
int totalTreeNodes = 0;
int maxNumOfDynamic = 0;
int maxDynamicDepth = 0;
float maxDynamicHalfWidth = 0;
int totalDynamicObjects = 0;
int maxNumOfStatic = 0;
int maxStaticDepth = 0;
float maxStaticHalfWidth = 0;
int totalStaticObjects = 0;
#endif

Prism::TreeNode::TreeNode(const CU::Vector3<float>& aPosition, float aHalfWidth, TreeNode* aParent
		, int aDepth, int aMaxDepth)
	: myPosition(aPosition)
	, myHalfWidth(aHalfWidth)
	, myParent(aParent)
	, myDepth(aDepth)
	, myMaxDepth(aMaxDepth)
	, myLooseness(1.f)
	, myObjectsDynamic(64)
	, myObjectsStatic(256)
	, myContainsObject(false)
{
	myLooseWidth = myHalfWidth * myLooseness;

	myChildren[0] = nullptr;
	myChildren[1] = nullptr;
	myChildren[2] = nullptr;
	myChildren[3] = nullptr;
	myChildren[4] = nullptr;
	myChildren[5] = nullptr;
	myChildren[6] = nullptr;
	myChildren[7] = nullptr;
}

Prism::TreeNode::~TreeNode()
{
	for (int i = 0; i < 8; ++i)
	{
		delete myChildren[i];
	}
}

void Prism::TreeNode::Update()
{
	for (int i = myObjectsDynamic.Size() - 1; i >= 0; --i)
	{
		Instance* object = myObjectsDynamic[i];
		if (myParent != nullptr && CheckEnclosed(object) == false)
		{
			myObjectsDynamic.RemoveCyclicAtIndex(i);
			myParent->InsertObjectUp(object);
		}
		else
		{
			myObjectsDynamic.RemoveCyclicAtIndex(i);
			InsertObjectDown(object);
		}
	}

#ifdef SHOW_OCTREE_DEBUG
	totalStaticObjects += myObjectsStatic.Size();
	totalDynamicObjects += myObjectsDynamic.Size();
#endif

	for (int i = 0; i < 8; ++i)
	{
		if (myChildren[i] != nullptr)
		{
			myChildren[i]->Update();
		}
	}

	bool containsObject = false;

	for (int i = 0; i < 8; ++i)
	{
		if (myChildren[i] != nullptr && myChildren[i]->myContainsObject == true)
		{
			containsObject = true;
			break;
		}
		else if (myChildren[i] != nullptr)
		{
			delete myChildren[i];
			myChildren[i] = nullptr;
		}
	}

	myContainsObject = containsObject|| myObjectsDynamic.Size() > 0 || myObjectsStatic.Size() > 0;
}

void Prism::TreeNode::GetOccupantsInAABB(const Frustum& aFrustum
	, CU::GrowingArray<Instance*>& aOutArray)
{
#ifdef SHOW_OCTREE_DEBUG
	if (myDepth == 0)
	{
		ss << "Total: " << totalTreeNodes;
		ss2 << "Max Dynamic: " << maxNumOfDynamic << " depth: " << maxDynamicDepth << " halfWidth: " << maxDynamicHalfWidth;
		ss3 << "Max Static: " << maxNumOfStatic << " depth: " << maxStaticDepth << " halfWidth: " << maxStaticHalfWidth;
		ss4 << "Total Dynamic: " << totalDynamicObjects;
		ss5 << "Total Static: " << totalStaticObjects;
		Engine::GetInstance()->PrintText(ss.str(), { 700.f, -700.f });
		Engine::GetInstance()->PrintText(ss2.str(), { 700.f, -730.f });
		Engine::GetInstance()->PrintText(ss3.str(), { 700.f, -760.f });
		Engine::GetInstance()->PrintText(ss4.str(), { 700.f, -790.f });
		Engine::GetInstance()->PrintText(ss5.str(), { 700.f, -820.f });
		ss.clear();
		ss.str(std::string());
		ss2.clear();
		ss2.str(std::string());
		ss3.clear();
		ss3.str(std::string());
		ss4.clear();
		ss4.str(std::string());
		ss5.clear();
		ss5.str(std::string());
		totalTreeNodes = 0;
		maxNumOfDynamic = 0;
		maxNumOfStatic = 0;
		totalDynamicObjects = 0;
		totalStaticObjects = 0;
	}
	++totalTreeNodes;
	if (myObjectsDynamic.Size() > maxNumOfDynamic)
	{
		maxNumOfDynamic = myObjectsDynamic.Size();
		maxDynamicDepth = myDepth;
		maxDynamicHalfWidth = myHalfWidth;
	}
	if (myObjectsStatic.Size() > maxNumOfStatic)
	{
		maxNumOfStatic = myObjectsStatic.Size();
		maxStaticDepth = myDepth;
		maxStaticHalfWidth = myHalfWidth;
	}

#endif
	for (int i = 0; i < myObjectsDynamic.Size(); ++i)
	{
		if (aFrustum.Inside(myObjectsDynamic[i]->GetPosition(), myObjectsDynamic[i]->GetObjectCullingRadius()) == true)
		{
			aOutArray.Add(myObjectsDynamic[i]);
		}
	}
	for (int i = 0; i < myObjectsStatic.Size(); ++i)
	{
		if (aFrustum.Inside(myObjectsStatic[i]->GetPosition(), myObjectsStatic[i]->GetObjectCullingRadius()) == true)
		{
			aOutArray.Add(myObjectsStatic[i]);
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		if (myChildren[i] != nullptr
			&& CU::Intersection::AABBvsAABB(myChildren[i]->GetMinCorner()
			, myChildren[i]->GetMaxCorner()
			, aFrustum.GetCornerMin()
			, aFrustum.GetCornerMax()) == true)
		{
#ifdef SHOW_OCTREE_DEBUG
			ss << i << " ";
#endif
			myChildren[i]->GetOccupantsInAABB(aFrustum, aOutArray);
		}
	}
}

void Prism::TreeNode::Modify(Instance* anObject, eModifyType aModifyType)
{
	bool straddle = false;
	int childIndex = 0;

	for (int i = 0; i < 3; ++i)
	{
		float delta = 0;

		if (i == 0)
		{
			delta = anObject->GetPosition().x - myPosition.x;
		}
		else if (i == 1)
		{
			delta = anObject->GetPosition().y - myPosition.y;
		}
		else if (i == 2)
		{
			delta = anObject->GetPosition().z - myPosition.z;
		}

		if (abs(delta) + myHalfWidth * (myLooseness - 1.f) <= anObject->GetObjectCullingRadius())
		{
			straddle = true;
			break;
		}

		if (delta > 0.0f)
		{
			childIndex |= (1 << i);
		}
	}

	if (straddle == false && myDepth < myMaxDepth - 1)
	{
		if (myChildren[childIndex] == nullptr)
		{
			if (aModifyType != eModifyType::REMOVE)
			{
				myChildren[childIndex] = SpawnChild(childIndex);
			}
			else
			{
				//use again when remove is moved to end of LogicUpdate by LinusL && Niklas:
				//DL_ASSERT("Error: TreeNode not found to remove object.");
				return;
			}
		}

		myChildren[childIndex]->Modify(anObject, aModifyType);
	}
	else
	{
		if (anObject->GetOctreeType() == eOctreeType::DYNAMIC)
		{
			switch (aModifyType)
			{
			case eModifyType::INSERT:
				for (int i = 0; i < myObjectsDynamic.Size(); ++i)
				{
					if (myObjectsDynamic[i] == anObject)
					{
						DL_ASSERT("Tried to add dynamic object twice into octree");
					}
				}

				myObjectsDynamic.Add(anObject);
				break;
			case eModifyType::REMOVE:
				myObjectsDynamic.RemoveCyclic(anObject);
				break;
			}
		}
		else if (anObject->GetOctreeType() == eOctreeType::STATIC)
		{
			switch (aModifyType)
			{
			case eModifyType::INSERT:
				for (int i = 0; i < myObjectsStatic.Size(); ++i)
				{
					if (myObjectsStatic[i] == anObject)
					{
						DL_ASSERT("Tried to add static object twice into octree");
					}
				}

				myObjectsStatic.Add(anObject);
				break;
			case eModifyType::REMOVE:
				myObjectsStatic.RemoveCyclic(anObject);
				break;
			}
		}
		else
		{
			DL_ASSERT("Unknown octree type.");
		}
	}
}

Prism::TreeNode* Prism::TreeNode::SpawnChild(int anId)
{
	CU::Vector3<int> dir;

	switch (anId)
	{
	case 0:
		dir = CU::Vector3<int>(-1, -1, -1);
		break;
	case 1:
		dir = CU::Vector3<int>(+1, -1, -1);
		break;
	case 2:
		dir = CU::Vector3<int>(-1, +1, -1);
		break;
	case 3:
		dir = CU::Vector3<int>(+1, +1, -1);
		break;
	case 4:
		dir = CU::Vector3<int>(-1, -1, +1);
		break;
	case 5:
		dir = CU::Vector3<int>(+1, -1, +1);
		break;
	case 6:
		dir = CU::Vector3<int>(-1, +1, +1);
		break;
	case 7:
		dir = CU::Vector3<int>(+1, +1, +1);
		break;
	default:
		DL_ASSERT("bad index!");
		break;
	}

	CU::Vector3<float> pos(myPosition);
	pos.x += dir.x * myHalfWidth / 2.f;
	pos.y += dir.y * myHalfWidth / 2.f;
	pos.z += dir.z * myHalfWidth / 2.f;
	return new TreeNode(pos, myHalfWidth / 2.f, this, myDepth + 1, myMaxDepth);
}

bool Prism::TreeNode::NodeVsAABB(const CU::Intersection::AABB& aAABB) const
{
	CU::Vector3<float> myCenter(myPosition.x, myPosition.y, myPosition.z);

	CU::Vector3<float> otherCenter(aAABB.myMinPos.x + aAABB.myExtents.x / 2.f, aAABB.myMinPos.y + aAABB.myExtents.y / 2.f, aAABB.myMinPos.z + aAABB.myExtents.z / 2.f);


	if (abs(myCenter.x - otherCenter.x) > (myHalfWidth + aAABB.myExtents.x / 2.f)) return false;
	if (abs(myCenter.y - otherCenter.y) > (myHalfWidth + aAABB.myExtents.y / 2.f)) return false;
	if (abs(myCenter.z - otherCenter.z) > (myHalfWidth + aAABB.myExtents.z / 2.f)) return false;

	return true;
}

bool Prism::TreeNode::CheckEnclosed(Instance* anObject) const
{
	return myPosition.x - myHalfWidth < anObject->GetPosition().x - anObject->GetObjectCullingRadius()
		&& myPosition.y - myHalfWidth < anObject->GetPosition().y - anObject->GetObjectCullingRadius()
		&& myPosition.z - myHalfWidth < anObject->GetPosition().z - anObject->GetObjectCullingRadius()
		&& myPosition.x + myHalfWidth > anObject->GetPosition().x + anObject->GetObjectCullingRadius()
		&& myPosition.y + myHalfWidth > anObject->GetPosition().y + anObject->GetObjectCullingRadius()
		&& myPosition.z + myHalfWidth > anObject->GetPosition().z + anObject->GetObjectCullingRadius();
}

void Prism::TreeNode::InsertObjectUp(Instance* anObject)
{
	if (myParent != nullptr && CheckEnclosed(anObject) == false)
	{
		myParent->InsertObjectUp(anObject);
	}
	else
	{
		InsertObjectDown(anObject);
	}
}