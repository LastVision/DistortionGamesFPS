#pragma once

#include "Component.h"

#include <Subscriber.h>

class SoundComponent : public Component, public Subscriber
{
public:
	SoundComponent(Entity& aEntity);
	~SoundComponent();

	void Update(float aDeltaTime) override;

	int GetAudioSFXID();
	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void ReceiveMessage(const LevelLoadedMessage& aMessage) override;
	void ReceiveMessage(const LevelCompleteMessage& aMessage) override;
private:
	int myAudioSFXID;
};

inline int SoundComponent::GetAudioSFXID()
{
	return myAudioSFXID;
}

inline eComponentType SoundComponent::GetTypeStatic()
{
	return eComponentType::SOUND;
}

inline eComponentType SoundComponent::GetType()
{
	return GetTypeStatic();
}

