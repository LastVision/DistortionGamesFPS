#include "stdafx.h"

#include "../Game/GameDefines.h"
#include "AnimationComponent.h"
#include <AudioInterface.h>
#include "Camera.h"
#include "HealthComponent.h"
#include "../Game/FogOfWarMap.h"
#include <Engine.h>
#include "../GUI/BarWidget.h"
#include "ModelLoader.h"
#include <MinimapEventMessage.h>
#include "EntityId.h"
#include <EmitterMessage.h>
#include <PostMaster.h>
#include "PollingStation.h"
#include "SoundComponent.h"
#include "TotemComponent.h"


#include "../Game/FogOfWarMap.h"

HealthComponent::HealthComponent(Entity& aEntity, HealthComponentData& aData)
	: Component(aEntity)
	, myMaxHealth(aData.myHealth)
	, myCurrentHealth(aData.myHealth)
	, myArmor(aData.myArmor)
	, myIsHealing(false)
{
	CU::Vector4<float> color = { 1.f, 1.f, 1.f, 1.f };

	switch (myEntity.GetOwner())
	{
	case eOwnerType::PLAYER:
		color = PLAYER_COLOR;
		break;
	case eOwnerType::ENEMY:
		color = { 1.f, 0.f, 0.f, 1.f };
		break;
	case eOwnerType::NEUTRAL:
		color = { 0.5f, 0.5f, 0.f, 1.f };
		break;
	default:
		break;
	}

	myHealthBar = new GUI::BarWidget(myMaxHealth, myCurrentHealth, { 50.f, 10.f }, color);
}

HealthComponent::~HealthComponent()
{
	SAFE_DELETE(myHealthBar);
}

void HealthComponent::RenderHealthBar(const Prism::Camera& aCamera)
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
	newRenderPos.y += 50.f;
	myHealthBar->Render({ newRenderPos.x, newRenderPos.y });
}

void HealthComponent::Update(float aDelta)
{
	aDelta;
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (myCurrentHealth == myMaxHealth)
		{
			myIsHealing = false;
		}
		else
		{
			myIsHealing = PollingStation::GetInstance()->GetTotem()->GetComponent<TotemComponent>()->GetIsInside(&myEntity);
		}

	}
}

bool HealthComponent::TakeDamageAndCheckSurvive(float aDamage)
{
	DL_ASSERT_EXP(aDamage >= 0, "Cant take negative damage, use Heal for healing if that was your intention");



	if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition())) 
	{
		if (myEntity.GetOwner() == eOwnerType::PLAYER)
		{
			PostMaster::GetInstance()->SendMessage(MinimapEventMessage(myEntity.GetPosition(), MinimapEventType::eUNIT_ATTACKED));
		}
		CU::Vector3<float> pos = myEntity.GetOrientation().GetPos();
		pos.y += 2;
		PostMaster::GetInstance()->SendMessage(EmitterMessage("OnHit", pos));
	}
	float damage = aDamage - myArmor;
	if (damage <= 0.f)
	{
		damage = 1.f;
	}

	if (myEntity.GetAlive() == false)
	{
		return true; // is this funky?
	}

	myCurrentHealth -= damage;
	if (myCurrentHealth <= 0)
	{
		myCurrentHealth = 0;
		myEntity.Kill();

		if (myEntity.GetUnitType() == eUnitType::GRUNT)
		{
			if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Death"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
		}
		else if (myEntity.GetUnitType() == eUnitType::RANGER)
		{
			if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Death"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
		}
		else if (myEntity.GetUnitType() == eUnitType::TANK)
		{
			if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Death"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
		}
		myEntity.SetState(eEntityState::DIE);
		return false;
	}


	if (myEntity.GetUnitType() == eUnitType::GRUNT)
	{
		if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_TakeDamage"
				, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
		}
	}
	else if (myEntity.GetUnitType() == eUnitType::RANGER)
	{
		if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_TakeDamage"
				, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
		}
	}
	else if (myEntity.GetUnitType() == eUnitType::TANK)
	{
		if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_TakeDamage"
				, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
		}
	}

	myHealthBar->Update(0.f);

	return true;
}

void HealthComponent::Heal(float aHealing)
{
	DL_ASSERT_EXP(aHealing >= 0, "Cant heal negative damage.");

	myCurrentHealth += aHealing;
	if (myCurrentHealth > myMaxHealth)
	{
		myCurrentHealth = myMaxHealth;
	}
	myHealthBar->Update(0.f);
}

void HealthComponent::Reset()
{
	myCurrentHealth = myMaxHealth;
	myHealthBar->Update(0.f);
}