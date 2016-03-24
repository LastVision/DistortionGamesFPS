#pragma once

namespace Prism
{
	class Camera;
}

struct NotificationText
{
	std::string myText;
	CU::Vector4<float> myColor;
	float myLifeTime;
	bool myIsActive;
};

struct MissionText
{
	std::string myText;
	CU::Vector4<float> myColor;
	int myMissionGID;
	bool myIsActive;
};

class TextEventManager
{
public:
	TextEventManager(const Prism::Camera* aCamera);
	~TextEventManager();

	void Update(float aDeltaTime);
	void Render();

	void AddNotification(std::string aText, float aLifeTime = 3.f, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f });

private:

	const Prism::Camera* myCamera;

	CU::GrowingArray<NotificationText*> myNotifications;
	CU::GrowingArray<MissionText*> myMissionTexts;

	float myTextStartFadingTime;
};

inline void TextEventManager::AddNotification(std::string aText, float aLifeTime, CU::Vector4<float> aColor)
{
	myNotifications[0]->myText = aText;
	myNotifications[0]->myLifeTime = aLifeTime;
	myNotifications[0]->myColor = aColor;
	myNotifications[0]->myIsActive = true;
}