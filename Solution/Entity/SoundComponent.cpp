#include "stdafx.h"
#include <AudioInterface.h>
#include "SoundComponent.h"
#include <LevelLoadedMessage.h>
#include <LevelCompleteMessage.h>
#include <PostMaster.h>



SoundComponent::SoundComponent(Entity& aEntity)
	: Component(aEntity)
	, myAudioSFXID(-1)
{
	myAudioSFXID = Prism::Audio::AudioInterface::GetInstance()->GetUniqueID();
	PostMaster::GetInstance()->Subscribe(eMessageType::LEVEL_LOADED, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::LEVEL_COMPLETE, this);
}

SoundComponent::~SoundComponent()
{
	if (!Prism::Audio::AudioInterface::GetInstance())
	{
		return;
	}
	Prism::Audio::AudioInterface::GetInstance()->UnRegisterObject(myAudioSFXID);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::LEVEL_LOADED, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::LEVEL_COMPLETE, this);
}

void SoundComponent::Update(float)
{
	Prism::Audio::AudioInterface::GetInstance()->SetPosition(myEntity.GetOrientation().GetPos().x
		, myEntity.GetOrientation().GetPos().y, myEntity.GetOrientation().GetPos().z, myAudioSFXID);
}

void SoundComponent::ReceiveMessage(const LevelLoadedMessage& aMessage)
{
	if (myEntity.GetSubType() == "sm_elevator_a_open")
	{
		std::string eventName("Play_3DElevatorToLevel" + std::to_string(min(2, aMessage.myLevelID)));
		Prism::Audio::AudioInterface::GetInstance()->PostEvent(eventName.c_str(), myAudioSFXID);
	}
	if (myEntity.GetSubType() == "sm_double_doors_a_400_300_laser")
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Door", myAudioSFXID);
	}
}

void SoundComponent::ReceiveMessage(const LevelCompleteMessage& aMessage)
{
	if (myEntity.GetSubType() == "sm_elevator_a_open")
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_3DAllElevators", myAudioSFXID);
	}
	if (myEntity.GetSubType() == "sm_double_doors_a_400_300_laser")
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Door", myAudioSFXID);
	}
}
