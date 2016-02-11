#include "stdafx.h"

#include <Engine.h> //WHy neeeded include??
#include "Effect.h"
#include "Instance.h"
#include <ModelLoader.h>
#include <ModelProxy.h>
#include "SelectionComponent.h"
#include "SelectionComponentData.h"
#include <Scene.h>


SelectionComponent::SelectionComponent(Entity& aEntity, SelectionComponentData& aComponentData)
	: Component(aEntity)
	, mySelectedInstance(nullptr)
	, myHoveredInstance(nullptr)
	, myCullingRadius(2.5f)
	, myPreviousHover(false)
{
	Prism::ModelProxy* selected = Prism::ModelLoader::GetInstance()->LoadModel(aComponentData.mySelectedPath
		, aComponentData.myEffectPath);
	mySelectedInstance = new Prism::Instance(*selected, myEntity.GetOrientation(), myEntity.GetOctreeType(), myCullingRadius);

	Prism::ModelProxy* hovered = Prism::ModelLoader::GetInstance()->LoadModel(aComponentData.myHoveredPath
		, aComponentData.myEffectPath);
	myHoveredInstance = new Prism::Instance(*hovered, myEntity.GetOrientation(), myEntity.GetOctreeType(), myCullingRadius);

	mySelectedInstance->SetScale(CU::Vector3<float>(aComponentData.myScale, aComponentData.myScale, aComponentData.myScale));
	myHoveredInstance->SetScale(CU::Vector3<float>(aComponentData.myScale, aComponentData.myScale, aComponentData.myScale));
}


SelectionComponent::~SelectionComponent()
{
	SAFE_DELETE(mySelectedInstance);
	SAFE_DELETE(myHoveredInstance);
}

void SelectionComponent::Update(float)
{
	mySelectedInstance->SetShouldRender(false);
	myHoveredInstance->SetShouldRender(false);

	if (myEntity.IsSelected() == true)
	{
		mySelectedInstance->SetShouldRender(true);
	}
	else if (myEntity.IsHovered() == true)
	{
		myHoveredInstance->SetShouldRender(true);
	}
}