#include "stdafx.h"
#include <InputWrapper.h>
#include "CinematicState.h"
#include <Sprite.h>
#include <SpriteProxy.h>
#include <Text.h>


CinematicState::CinematicState(const std::string& aScenePath)
	: myTimer(FLT_MAX)
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	CU::Vector2<float> backgroundSize(2048.f, 1024.f);

	CinematicScene toAdd;
	toAdd.myBackground = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Cinematic/test_background.dds",
		backgroundSize, windowSize * 0.5f);
	toAdd.myFirstImage = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Cinematic/test_left.dds",
		backgroundSize, windowSize * 0.5f);

	toAdd.mySecondImage = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Cinematic/test_right.dds",
		backgroundSize, windowSize * 0.5f);


	toAdd.myBackgroundPosition = windowSize * 0.5f;
	toAdd.myFirstImagePosition = windowSize * 0.5f;
	toAdd.myFirstImagePosition = windowSize * 0.5f;
	myScenes.push(toAdd);

	ReadScenes(aScenePath);
}


CinematicState::~CinematicState()
{

}

void CinematicState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	aCursor;
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;

	OnResize(Prism::Engine::GetInstance()->GetWindowSizeInt().x, Prism::Engine::GetInstance()->GetWindowSizeInt().y);
	myTimer = myScenes.front().mySceneTime;
}

void CinematicState::EndState()
{

}

const eStateStatus CinematicState::Update(const float& aDeltaTime)
{
	myTimer -= aDeltaTime;
	if (myTimer < 0.f)
	{
		myScenes.pop();
		myTimer = myScenes.front().mySceneTime;
	}
	myScenes.front().myBackground;
	myScenes.front().myFirstImagePosition.x -= myScenes.front().myFirstImagePanningSpeed * aDeltaTime;
	myScenes.front().mySecondImagePosition.x -= myScenes.front().mySecondImagePanningSpeed * aDeltaTime;

	if (myScenes.empty() == true || CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_M))
	{

		return eStateStatus::ePopMainState;
	}

	

	return eStateStatus::eKeepState;
}

void CinematicState::Render()
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(Prism::Engine::GetInstance()->GetWindowSize().y));
	myScenes.front().myBackground->Render(myScenes.front().myBackgroundPosition);
	myScenes.front().myFirstImage->Render(myScenes.front().myFirstImagePosition);
	myScenes.front().mySecondImage->Render(myScenes.front().mySecondImagePosition);

}

void CinematicState::ResumeState()
{

}

void CinematicState::OnResize(int aWidth, int aHeight)
{
	aWidth;
	aHeight;
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	if (myScenes.front().myBackground != nullptr)
	{
		myScenes.front().myBackground->SetSize(windowSize, windowSize / 2.f);
		myScenes.front().myFirstImage->SetSize(windowSize, windowSize / 2.f);
		myScenes.front().mySecondImage->SetSize(windowSize, windowSize / 2.f);
	}
}

void CinematicState::ReadScenes(const std::string& aScenePath)
{
	aScenePath;
}

