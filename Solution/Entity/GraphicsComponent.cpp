#include "stdafx.h"

#include "GraphicsComponent.h"
#include <Effect.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <EffectContainer.h>
#include <Instance.h>
#include <Model.h>
#include <ModelLoader.h>
#include <ModelProxy.h>
#include <Scene.h>
#include <Texture.h>

GraphicsComponent::GraphicsComponent(Entity& aEntity, const GraphicsComponentData& aComponentData)
	: Component(aEntity)
	, myComponentData(aComponentData)
	, myInstance(nullptr)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel(myComponentData.myModelPath
		, myComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation);
}

GraphicsComponent::~GraphicsComponent()
{
	myEntity.GetScene().RemoveInstance(myInstance);
	delete myInstance;
	myInstance = nullptr;
}

void GraphicsComponent::InitDLL(const char* aModelPath, const char* aEffectPath)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel(aModelPath
		, aEffectPath);

	Prism::EffectContainer::GetInstance()->GetEffect(aEffectPath);
	model->SetEffect(Prism::EffectContainer::GetInstance()->GetEffect(aEffectPath));

	myInstance = new Prism::Instance(*model, myEntity.myOrientation);
}

void GraphicsComponent::InitCube(float aWidth, float aHeight, float aDepth)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadCube(aWidth, aHeight, aDepth);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation);
}

void GraphicsComponent::Update(float aDeltaTime)
{
	aDeltaTime;
}

void GraphicsComponent::SetPosition(const CU::Vector3<float>& aPosition)
{
	myEntity.myOrientation.SetPos(aPosition);
}

void GraphicsComponent::SetRotation(const CU::Vector3<float>& aRotation)
{
	CU::Vector3f position(myEntity.myOrientation.GetPos());
	myEntity.myOrientation.SetPos(CU::Vector3f());

	myEntity.myOrientation = CU::Matrix44f::CreateRotateAroundX(aRotation.x) * myEntity.myOrientation;
	myEntity.myOrientation = CU::Matrix44f::CreateRotateAroundY(aRotation.y) * myEntity.myOrientation;
	myEntity.myOrientation = CU::Matrix44f::CreateRotateAroundZ(aRotation.z) * myEntity.myOrientation;

	myEntity.myOrientation.SetPos(position);
}

void GraphicsComponent::SetScale(const CU::Vector3<float>& aScale)
{
	myInstance->SetScale(aScale);
}