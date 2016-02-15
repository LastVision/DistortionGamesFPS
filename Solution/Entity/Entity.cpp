#include "stdafx.h"

#include "AnimationComponent.h"
#include "GraphicsComponent.h"
#include <Scene.h>
#include <Terrain.h>
#include <Instance.h>
#include <EmitterMessage.h>
#include <PostMaster.h>
Entity::Entity(eOwnerType aOwner, Prism::eOctreeType anOctreeType, EntityData& aEntityData
		, Prism::Scene& aScene, const CU::Vector3<float>& aStartPosition, const Prism::Terrain& aTerrain
		, const CU::Vector3f& aRotation, const CU::Vector3f& aScale, eUnitType aUnitType)
	: myOwner(aOwner)
	, myTemporaryOwner(aOwner)
	, myScene(aScene)
	, myOctreeType(anOctreeType)
	, myState(eEntityState::IDLE)
	, myType(aEntityData.myType)
	, myPosition({aStartPosition.x, aStartPosition.z})
	, myUnitType(aUnitType)
	, myEmitterConnection(nullptr)
	, myArtifactTotalTime((static_cast<float>(rand() % 100) * 0.01f))
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		myComponents[i] = nullptr;
	}

	myOrientation.SetPos(aStartPosition);
	myOriginalY = aStartPosition.y;

	if (aEntityData.myAnimationData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::ANIMATION)] = new AnimationComponent(*this, aEntityData.myAnimationData, aTerrain);
		GetComponent<AnimationComponent>()->SetRotation(aRotation);
		GetComponent<AnimationComponent>()->SetScale(aScale);
	}
	else if (aEntityData.myGraphicsData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::GRAPHICS)] = new GraphicsComponent(*this, aEntityData.myGraphicsData);
		GetComponent<GraphicsComponent>()->SetRotation(aRotation);
		GetComponent<GraphicsComponent>()->SetScale(aScale);
	}
	
	Reset();
}

Entity::~Entity()
{

	if (myIsInScene == true)
	{
		RemoveFromScene();
	}
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		delete myComponents[i];
		myComponents[i] = nullptr;
	}
}

void Entity::Reset()
{
	myState = eEntityState::IDLE;
	myAlive = false;
	myDecayFlag = false;
	myHovered = false;
	mySelected = false;
	myIsSelectable = true;
	myIsInScene = false;
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Reset();
		}
	}
}

void Entity::Update(float aDeltaTime)
{
	if (myType == eEntityType::ARTIFACT)
	{
		UpdateArtifact(aDeltaTime);
	}
	myPosition = { myOrientation.GetPos().x, myOrientation.GetPos().z };
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Update(aDeltaTime);
		}
	}

	if (myDecayFlag == true)
	{
		myCurrentDecayTime -= aDeltaTime;
		myOrientation.SetPos({ myPosition.x, myOrientation.GetPos().y - 0.05f * aDeltaTime, myPosition.y });
		if (myCurrentDecayTime < 0)
		{
			myCurrentDecayTime = 0;
		}
	}

	//if (mySelected == true)
	//{
	//	Prism::RenderBox(myOrientation.GetPos());
	//	//mySelectionCircle.Render(myOrientation.GetPos());
	//}
	//else if (myHovered == true)
	//{
	//	Prism::RenderBox(myOrientation.GetPos(), eColorDebug::WHITE);
	//}
	if (myComponents[static_cast<int>(eComponentType::ANIMATION)] != nullptr && myAlive == false && myState == eEntityState::DIE &&
		static_cast<AnimationComponent*>(myComponents[static_cast<int>(eComponentType::ANIMATION)])->IsCurrentAnimationDone() 
		&& myDecayFlag == false)
	{
		myCurrentDecayTime = 30.f;
		myDecayFlag = true;
	}
}

void Entity::UpdateArtifact(float aDeltaTime)
{
	myArtifactTotalTime += aDeltaTime;
	float heightToAdd = sinf(myArtifactTotalTime * 4.f) * 0.3f;
	CU::Vector3<float> pos(myOrientation.GetPos());
	pos.y = myOriginalY;
	pos.y += heightToAdd;
	pos.y += 0.3f;

	myOrientation.SetPos(CU::Vector3<float>());

	myOrientation = myOrientation * CU::Matrix44<float>::CreateRotateAroundY(aDeltaTime);
	myOrientation.SetPos(pos);
}

void Entity::AddComponent(Component* aComponent)
{
	DL_ASSERT_EXP(myComponents[int(aComponent->GetType())] == nullptr, "Tried to add component several times");
	myComponents[int(aComponent->GetType())] = aComponent;
}

void Entity::RemoveComponent(eComponentType aComponent)
{
	DL_ASSERT_EXP(myComponents[int(aComponent)] != nullptr, "Tried to remove an nonexisting component");
	delete myComponents[int(aComponent)];
	myComponents[int(aComponent)] = nullptr;
}

void Entity::AddToScene()
{
	if (GetComponent<GraphicsComponent>() != nullptr && GetComponent<GraphicsComponent>()->GetInstance() != nullptr)
	{
		myScene.AddInstance(GetComponent<GraphicsComponent>()->GetInstance());
	}
	else if (GetComponent<AnimationComponent>() != nullptr && GetComponent<AnimationComponent>()->GetInstance() != nullptr)
	{
		myScene.AddInstance(GetComponent<AnimationComponent>()->GetInstance());
	}
	
	myIsInScene = true;
}

void Entity::RemoveFromScene()
{
	if (GetComponent<GraphicsComponent>() != nullptr)
	{
		myScene.RemoveInstance(GetComponent<GraphicsComponent>()->GetInstance());
	}
	else if (GetComponent<AnimationComponent>() != nullptr)
	{
		myScene.RemoveInstance(GetComponent<AnimationComponent>()->GetInstance());
	}

	myIsInScene = false;
}

void Entity::RemoveSelectionRingFromScene()
{
	//int IS_THIS_NEEDED = 5;
	/*if (GetComponent<SelectionComponent>() != nullptr)
	{
		myScene.RemoveInstance(GetComponent<SelectionComponent>()->GetInstance());
		GetComponent<SelectionComponent>()->SetIsRemovedFromScene(true);
	}*/
}

void Entity::Kill()
{
	DL_ASSERT_EXP(myAlive == true, "Tried to kill an Entity multiple times");
	myAlive = false;
	//RemoveFromScene();
}

void Entity::Spawn(const CU::Vector2<float>& aSpawnPoint, const CU::Vector2<float>& aRallyPoint)
{
	myOrientation.SetPos(CU::Vector3<float>(aSpawnPoint.x, 0, aSpawnPoint.y));
	myPosition = aSpawnPoint;
	Reset();
	myAlive = true;
	AddToScene();
}

void Entity::SetSelect(bool aStatus)
{
	mySelected = aStatus;
}

bool Entity::IsSelected() const
{
	return mySelected;
}

void Entity::SetHovered(bool aStatus)
{
	myHovered = aStatus;
}

bool Entity::IsHovered() const
{
	return myHovered;
}

void Entity::SetShouldRender(bool aStatus)
{
	if (GetComponent<AnimationComponent>() != nullptr)
	{
		GetComponent<AnimationComponent>()->GetInstance()->SetShouldRender(aStatus);
	}

	if (GetComponent<GraphicsComponent>() != nullptr)
	{
		GetComponent<GraphicsComponent>()->GetInstance()->SetShouldRender(aStatus);
	}
}

bool Entity::GetShouldBeRemoved() const
{
	return myAlive == false && myState == eEntityState::DIE && myCurrentDecayTime == 0 && myDecayFlag == true &&
		static_cast<AnimationComponent*>(myComponents[static_cast<int>(eComponentType::ANIMATION)])->IsCurrentAnimationDone();
}

CU::GrowingArray<CU::Vector2<float>> Entity::GetCutMesh() const
{
	CU::GrowingArray<CU::Vector2<float>> points(4);

	CU::Vector2<float> pos(myOrientation.GetPos().x, myOrientation.GetPos().z);

	float halfWidth = 0.5f;
	
	points.Add({ pos.x - halfWidth, pos.y - halfWidth });
	points.Add({ pos.x - halfWidth, pos.y + halfWidth });
	points.Add({ pos.x + halfWidth, pos.y + halfWidth });
	points.Add({ pos.x + halfWidth, pos.y - halfWidth });
	return points;
}

void Entity::AddEmitter(Prism::ParticleEmitterInstance* anEmitterConnection)
{
	myEmitterConnection = anEmitterConnection;
}

Prism::ParticleEmitterInstance* Entity::GetEmitter()
{
	return myEmitterConnection;
}

void Entity::SetPosition(const CU::Vector3f& aPosition)
{
	myOrientation.SetPos(aPosition);
	myPosition.x = aPosition.x;
	myPosition.y = aPosition.z;
}