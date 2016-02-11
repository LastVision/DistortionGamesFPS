#pragma once
#include <Subscriber.h>

namespace Prism
{
	class Text;
	class Camera;
}

struct InWorldText
{
	Prism::Text* myText;
	CU::Vector3<float> myInWorldPosition;
	CU::Vector4<float> myColor;
	float myLifeTime;
	bool myIsActive;
};

struct NotificationText
{
	Prism::Text* myText;
	CU::Vector4<float> myColor;
	float myLifeTime;
	bool myIsActive;
};

class TextEventManager : public Subscriber
{
public:
	TextEventManager(const Prism::Camera* aCamera);
	~TextEventManager();

	void Update(float aDeltaTime);
	void Render();

	void AddNotification(const std::string& aText, const CU::Vector4<float>& aColor = { 1.f, 1.f, 1.f, 1.f });
	void AddInWorldText(const std::string& aText, const CU::Vector3<float>& aPosition, const CU::Vector4<float>& aColor = { 1.f, 1.f, 1.f, 1.f });
	void AddInWorldText(const std::string& aText, const CU::Vector2<float>& aPosition, const CU::Vector4<float>& aColor = { 1.f, 1.f, 1.f, 1.f });

	void ReceiveMessage(const NotificationMessage& aMessage) override;
	void ReceiveMessage(const InWorldTextMessage& aMessage) override;

	void OnResize(const CU::Vector2<float>& aNewSize);

private:

	CU::Vector2<float> Get2DPosition(const CU::Vector3<float>& aPosition);

	CU::GrowingArray<InWorldText*> myInWorldTexts;
	CU::GrowingArray<NotificationText*> myNotifications;

	CU::Vector2<float> myNotificationPosition;
	CU::Vector2<float> mySize;

	float myTextLifeTime;
	float myTextStartFadingTime;
	float myTextScale;

	int myInWorldTextIndex;
	int myNotificationIndex;
	int myInWorldTextMax;
	int myNotificationMax;

	const Prism::Camera* myCamera;
};

