#pragma once
#include <NetworkSubscriber.h>
#include <Subscriber.h>

namespace Prism
{
	class Camera;
	class TextProxy;
}

struct NotificationText
{
	Prism::TextProxy* my3dText;
	std::string myCurrentText;
	std::string myRemainingText;
	CU::Vector4<float> myColor;
	float myLifeTime;
	float myNextLetterInterval;
	float myCurrentLetterInterval;
	bool myIsActive;
	int myTextRows;
};

class TextEventManager : public NetworkSubscriber, public Subscriber
{
public:
	TextEventManager(const Prism::Camera* aCamera);
	~TextEventManager();

	void Update(float aDeltaTime);
	void Render();

	void AddNotification(std::string aText, float aLifeTime = 5.f, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f }, int aNumberOfRows = 0);
	void AddRespawnText(std::string aText, bool aShouldShow, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f });

	void ReceiveNetworkMessage(const NetMessageText& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveMessage(const PrintTextMessage& aMessage) override;

private:

	const Prism::Camera* myCamera;

	CU::GrowingArray<NotificationText*> myNotifications;
	CU::Vector3<float> myStartOffset;

	float myTextStartFadingTime;
	Prism::TextProxy* myMissionText;
	Prism::TextProxy* myRespawnText;
	CU::Vector3<float> myMissionOffset;
	CU::Vector3<float> myRespawnOffset;
	bool myShouldRender;
	bool myShouldRenderRespawn;
	float myRespawnTimer;
	CU::Vector4<float> myRespawnColor;

	bool myHasStoppedSound[8];
};