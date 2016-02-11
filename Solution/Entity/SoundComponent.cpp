#include "stdafx.h"

#include <AudioInterface.h>
#include "SoundComponent.h"


SoundComponent::SoundComponent(Entity& aEntity)
	: Component(aEntity)
	, myAudioSFXID(-1)
{
	myAudioSFXID = Prism::Audio::AudioInterface::GetInstance()->GetUniqueID();
}


SoundComponent::~SoundComponent()
{
	Prism::Audio::AudioInterface::GetInstance()->UnRegisterObject(myAudioSFXID);
}

void SoundComponent::Update(float)
{
	Prism::Audio::AudioInterface::GetInstance()->SetPosition(myEntity.GetOrientation().GetPos().x
		, myEntity.GetOrientation().GetPos().y, myEntity.GetOrientation().GetPos().z, myAudioSFXID);
}
