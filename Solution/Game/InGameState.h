#pragma once
#include "GameState.h"
#include <Matrix.h>
#include <GrowingArray.h>
#include <Subscriber.h>

class Level;
class LevelFactory;
class MessageState;

namespace CU
{
	class InputWrapper;
}

namespace Prism
{
	class Camera;
	class Sprite;
}

namespace GUI
{
	class GUIManager;
	class Cursor;
}

class InGameState : public GameState, public Subscriber
{
public:

	InGameState(int aLevelindex, eDifficulty aDifficulty);
	~InGameState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const GameStateMessage& aMessage) override;
	void ReceiveMessage(const OnClickMessage& aMessage) override;
	void ReceiveMessage(const MoveCameraMessage& aMessage) override;
	void ReceiveMessage(const CinematicMessage& aMessage) override;
	void ReceiveMessage(const TriggerMessage& aMessage) override;

	void SetLevel();

	void RestartLevel();
	void CompleteLevel();
	void CompleteGame();

	void LoadLevelSettings(); 
	void LoadPlayerSettings();

	bool IsComplete() const;

private:
	void UpdateCamera(float aDeltaTime, const CU::Vector3<float>& aCameraMovement);
	void ShowMessage(const std::string& aBackgroundPath, const CU::Vector2<float>& aSize, std::string aText, GameStateMessage* aMessage = nullptr);
	void CapCameraToTerrain();

	Level* myLevel;
	LevelFactory* myLevelFactory;
	Prism::Camera* myCamera;
	GUI::GUIManager* myGUIManager;

	MessageState* myMessageScreen;

	CU::Matrix44<float> myCameraOrientation;

	bool myIsComplete;
	bool myIsShuttingDown;
	bool myShouldReOpenConsole;
	bool myReturnedFromCredits;

	bool myIsPlayerCinematic;
	int myCinematicIndex;

	int myStartLevelIndex;
	eDifficulty myDifficulty;

	bool myIsFirstFrame;
};

inline bool InGameState::IsComplete() const
{
	return myIsComplete;
}