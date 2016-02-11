#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include <Engine.h>
#include <ModelLoader.h>
#include "PromotionComponent.h"
#include "PromotionComponentData.h"
#include <SpriteProxy.h>
#include <PostMaster.h>
#include <EmitterMessage.h>
#include "SoundComponent.h"

PromotionComponent::PromotionComponent(Entity& aEntity, PromotionComponentData& aData)
	: Component(aEntity)
	, myKillCount(aData.myKillCount)
	, myIsPromoted(false)
	, myCurrentKillCount(0)
{
	CU::Vector2<float> size(32.f, 32.f);
	mySprite = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/UI/T_unit_promotion.dds", size, size * 0.5f);
	Reset();
}

PromotionComponent::~PromotionComponent()
{
	SAFE_DELETE(mySprite);
}

void PromotionComponent::Reset()
{
	myCurrentKillCount = 0;
	myIsPromoted = false;
}

void PromotionComponent::RenderPromotion(const Prism::Camera& aCamera)
{
	if (GetPromoted() == true)
	{
		CU::Matrix44<float> renderPos;
		renderPos.SetPos(myEntity.GetOrientation().GetPos());
		renderPos = renderPos * CU::InverseSimple(aCamera.GetOrientation());
		renderPos = renderPos * aCamera.GetProjection();

		CU::Vector3<float> newRenderPos = renderPos.GetPos();
		CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();

		newRenderPos /= renderPos.GetPos4().w;
		newRenderPos += 1.f;
		newRenderPos *= 0.5f;
		newRenderPos.x *= windowSize.x;
		newRenderPos.y *= windowSize.y;
		newRenderPos.y += 66.f;
		mySprite->Render({ newRenderPos.x, newRenderPos.y });
	}
}

void PromotionComponent::RenderPromotion(const CU::Vector2<float>& aPosition, const CU::Vector2<float>& aScale, const CU::Vector4<float>& aColor)
{
	if (GetPromoted() == true)
	{
		mySprite->Render(aPosition, aScale, aColor);
	}
}

void PromotionComponent::Promote()
{
	if (myCurrentKillCount >= myKillCount)
	{
		PostMaster::GetInstance()->SendMessage(EmitterMessage("promotion", myEntity.GetId()));
		myIsPromoted = true;
		if (myEntity.GetOwner() == eOwnerType::PLAYER)
		{
			if (myEntity.GetComponent<SoundComponent>() != nullptr)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Promotion"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
		}
	}
}
