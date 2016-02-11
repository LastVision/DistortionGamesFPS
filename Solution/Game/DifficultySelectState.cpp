#include "stdafx.h"
#include <Camera.h>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "DifficultySelectState.h"
#include <OnClickMessage.h>
#include <PostMaster.h>
#include "StateStackProxy.h"
#include <FadeMessage.h>
#include <Cursor.h>
#include <ModelLoader.h>
#include <SpriteProxy.h>

DifficultySelectState::DifficultySelectState(int aLevelindex)
	: myLevelIndex(aLevelindex)
	, myGUIManager(nullptr)
	, myFadingGUIManager(nullptr)
	, myFadeTimer(0.f)
	, myIsFading(false)
	, myHasRenderedFadingGUI(false)
{
	CU::Vector2<float> logoSize(512.f, 512.f);
	myLogo = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/MainMenu/T_gamelogo.dds"
		, logoSize, logoSize * 0.5f);
	myLogoDust = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/MainMenu/T_gamelogo_dust.dds"
		, logoSize, logoSize * 0.5f);

	myWindowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	myLogoEndPosition.x = myWindowSize.x * 0.5f;
	myLogoEndPosition.y = myWindowSize.y - (myLogo->GetSize().y * 0.5f) + 25.f;
}

DifficultySelectState::~DifficultySelectState()
{
	SAFE_DELETE(myGUIManager);
	SAFE_DELETE(myFadingGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void DifficultySelectState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myIsActiveState = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_difficulty_select.xml", nullptr, nullptr, nullptr, -1);
	myFadingGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_loading_screen.xml", nullptr, nullptr, nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
	myFadeTimer = 1.f / 3.f;
	myIsFading = false;
	myHasRenderedFadingGUI = false;
}

void DifficultySelectState::EndState()
{

}

const eStateStatus DifficultySelectState::Update(const float& aDeltaTime)
{
	aDeltaTime;

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
	{
		myIsActiveState = false;
		return eStateStatus::ePopMainState;
	}

	

	if (myIsFading == true)
	{
		myFadeTimer -= aDeltaTime;
		if (myFadeTimer < 0.f)
		{
			myStateStack->PushSubGameState(new InGameState(myLevelIndex, myDifficulty));
			myFadeTimer = 0.f;
		}
	}
	else
	{
		myGUIManager->Update(aDeltaTime);
	}

	return myStateStatus;
}

void DifficultySelectState::Render()
{
	
	if (myIsFading == true)
	{
		myFadingGUIManager->Render();
	}
	else
	{
		myGUIManager->Render();

		myLogo->Render(myLogoEndPosition);
		myLogoDust->Render(myLogoEndPosition, CU::Vector2<float>(1.f, 1.f), CU::Vector4<float>(1.f, 1.f, 1.f, 1.f));
	}
}

void DifficultySelectState::ResumeState()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
	myFadeTimer = 1.f / 3.f;
	myIsFading = false;
	myHasRenderedFadingGUI = false;
}

void DifficultySelectState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
	myFadingGUIManager->OnResize(aWidth, aHeight);
}

void DifficultySelectState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::GAME_START_EASY:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myDifficulty = eDifficulty::EASY;
			myIsFading = true;
			PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
			myCursor->SetShouldRender(false);
			//myStateStack->PushSubGameState(new InGameState(myLevelIndex, eDifficulty::EASY));
			break;
		case eOnClickEvent::GAME_START_NORMAL:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myDifficulty = eDifficulty::NORMAL;
			myIsFading = true;
			PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
			myCursor->SetShouldRender(false);
			//myStateStack->PushSubGameState(new InGameState(myLevelIndex, eDifficulty::NORMAL));
			break;
		case eOnClickEvent::GAME_START_HARD:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myDifficulty = eDifficulty::HARD;
			myIsFading = true;
			PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
			myCursor->SetShouldRender(false);
			//myStateStack->PushSubGameState(new InGameState(myLevelIndex, eDifficulty::HARD));
			break;
		case eOnClickEvent::GAME_QUIT:
			myStateStatus = eStateStatus::ePopMainState;
			break;
		default:
			DL_ASSERT("Unknown event type.");
			break;
		}
	}
}