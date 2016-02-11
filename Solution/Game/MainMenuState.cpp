#include "stdafx.h"
#include <AudioInterface.h>
#include <Camera.h>
#include "CreditMenuState.h"
#include <FadeMessage.h>
#include <GUIManager.h>
#include "GameSettingsSingleton.h"
#include "HelpState.h"
#include "InGameState.h"
#include <InputWrapper.h>
#include "LevelSelectState.h"
#include "MainMenuState.h"
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include "OptionsState.h"
#include <PostMaster.h>
#include <PollingStation.h>
#include "SplashState.h"
#include "StateStackProxy.h"
#include <SpriteProxy.h>

#ifdef USE_DIFFICULTY
#include "DifficultySelectState.h"
#endif

MainMenuState::MainMenuState()
	: myLogoPosition(0.f, 0.f)
	, myLogoAlpha(0.f)
	, myMenuAlpha(0.f)
	, myDustAlpha(0.f)
	, myGUIPosition(0.f, 0.f)
	, myGUIEndPosition(0.f, 0.f)
	, myGUIStartPosition(-512.f, 0.f)
	, myLogoDone(false)
	, myGUIManager(nullptr)
	, myHasRunOnce(false)
{
	CU::Vector2<float> logoSize(512.f, 512.f);
	myLogo = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/MainMenu/T_gamelogo.dds"
		, logoSize, logoSize * 0.5f);
	myLogoDust = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/MainMenu/T_gamelogo_dust.dds"
		, logoSize, logoSize * 0.5f);

	myWindowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	myLogoPosition.x = myWindowSize.x * 0.5f;
	myLogoPosition.y = myWindowSize.y + (logoSize.y * 2.f);
	myLogoStartPosition = myLogoPosition;
	myLogoEndPosition.x = myLogoPosition.x;
	myLogoEndPosition.y = myWindowSize.y - (myLogo->GetSize().y * 0.5f);

	myGUIPosition = myGUIStartPosition;
}

MainMenuState::~MainMenuState()
{
	SAFE_DELETE(myLogo);
	SAFE_DELETE(myLogoDust);
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
	GameSettingsSingleton::Destroy();
}

void MainMenuState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myIsActiveState = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_main_menu.xml", nullptr, nullptr, nullptr, -1);
	myGUIManager->SetPosition(myGUIPosition);
	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
	myHasRunOnce = false;

	GameSettingsSingleton::GetInstance();

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void MainMenuState::EndState()
{
	
}

const eStateStatus MainMenuState::Update(const float& aDeltaTime)
{
	if (myHasRunOnce == false)
	{
#ifdef RELEASE_BUILD
		PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
		//bool runtime = ////Prism::MemoryTracker::GetInstance()->GetRunTime();
		////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_logo_other.dds", false));
		////Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
		//runtime = ////Prism::MemoryTracker::GetInstance()->GetRunTime();
		////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_logo_our.dds", true));
		////Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
#endif
		myHasRunOnce = true;
	}
	else 
	{
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
		{
			myIsActiveState = false;
			return eStateStatus::ePopMainState;
		}
		myLogoAlpha += aDeltaTime;
		if (myLogoPosition.y - 25.f > myLogoEndPosition.y)
		{
			myLogoPosition.y = myTweener.DoTween(myLogoAlpha, myLogoStartPosition.y, myLogoEndPosition.y - myLogoStartPosition.y, 1.5f, eTweenType::EXPONENTIAL_HALF);
		}
		else
		{
			myLogoPosition.y = myLogoEndPosition.y + 25.f;
			myMenuAlpha += aDeltaTime;
			if (myLogoDone == false)
			{
				myLogoDone = true;
				myMenuAlpha = 0.f;
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Logo_Thud", 0);
			}
			if (myGUIPosition.x < 0.f)
			{
				myGUIPosition.x = myTweener.DoTween(myMenuAlpha, myGUIStartPosition.x, myGUIEndPosition.x - myGUIStartPosition.x, 1.5f, eTweenType::EXPONENTIAL_HALF);
			}
		}

		myDustAlpha = fmaxf(myLogoAlpha - 2.5f, 0.f);
		myDustAlpha *= 3.f;
		myDustAlpha = fminf(myDustAlpha, 1.f);

		myGUIManager->SetPosition(myGUIPosition);
		myGUIManager->Update(aDeltaTime);
	}
	return myStateStatus;
}

void MainMenuState::Render()
{
	myGUIManager->Render();
	myLogo->Render(myLogoPosition);
	myLogoDust->Render({ myLogoEndPosition.x, myLogoEndPosition.y + 25.f }, CU::Vector2<float>(0.95f + myDustAlpha * 0.05f, 0.95f + myDustAlpha * 0.05f), CU::Vector4<float>(1.f, 1.f, 1.f, myDustAlpha));
}

void MainMenuState::ResumeState()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Menu", 0);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void MainMenuState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void MainMenuState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::GAME_START:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
#ifndef USE_DIFFICULTY
			myStateStack->PushMainGameState(new InGameState(aMessage.myID, eDifficulty::NORMAL));
#else
			myStateStack->PushMainGameState(new DifficultySelectState(aMessage.myID));
#endif
			break;
		case eOnClickEvent::GAME_LEVEL_SELECT:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushMainGameState(new LevelSelectState());
			break;
		case eOnClickEvent::GAME_CREDIT:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushSubGameState(new CreditMenuState());
			break;
		case eOnClickEvent::GAME_HELP:
		{
			//bool oldRuntime = ////Prism::MemoryTracker::GetInstance()->GetRunTime();
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushSubGameState(new HelpState());
			////Prism::MemoryTracker::GetInstance()->SetRunTime(oldRuntime);
			break;
		}
		case eOnClickEvent::OPTIONS_MENU:
		{
			//bool oldRuntime = ////Prism::MemoryTracker::GetInstance()->GetRunTime();
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushSubGameState(new OptionsState());
			////Prism::MemoryTracker::GetInstance()->SetRunTime(oldRuntime);
			break;
		}
		case eOnClickEvent::GAME_QUIT:
			myStateStatus = eStateStatus::ePopMainState;
			break;
		
		default:
			DL_ASSERT("Unknown event type.");
			break;
		}
	}
}