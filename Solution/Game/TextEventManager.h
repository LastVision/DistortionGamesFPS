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

private:

	const Prism::Camera* myCamera;

	CU::GrowingArray<NotificationText*> myNotifications;
	CU::GrowingArray<MissionText*> myMissionTexts;

	float myTextStartFadingTime;
};

