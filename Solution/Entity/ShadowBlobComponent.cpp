#include "stdafx.h"

#include <Engine.h> //WHy neeeded include??
#include "Effect.h"
#include "Instance.h"
#include <ModelLoader.h>
#include <ModelProxy.h>
#include "ShadowBlobComponent.h"
#include "ShadowBlobComponentData.h"
#include <Scene.h>


ShadowBlobComponent::ShadowBlobComponent(Entity& aEntity, ShadowBlobComponentData& aComponentData)
	: Component(aEntity)
	, myShadowBlob(nullptr)
	, myCullingRadius(2.5f)
{
	Prism::ModelProxy* modelProxy = Prism::ModelLoader::GetInstance()->LoadModel(aComponentData.myShadowBlobPath
		, aComponentData.myEffectPath);
	myShadowBlob = new Prism::Instance(*modelProxy, myEntity.GetOrientation(), myEntity.GetOctreeType(), myCullingRadius);

	myShadowBlob->SetScale(CU::Vector3<float>(aComponentData.myScale, aComponentData.myScale, aComponentData.myScale));
	myShadowBlob->SetShouldRender(true);

}


ShadowBlobComponent::~ShadowBlobComponent()
{
	SAFE_DELETE(myShadowBlob);
}

void ShadowBlobComponent::Update(float)
{
}