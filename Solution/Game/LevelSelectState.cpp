#include "stdafx.h"
#include <Camera.h>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "LevelSelectState.h"
#include <OnClickMessage.h>
#include <PostMaster.h>
#include "StateStackProxy.h"
#include <FadeMessage.h>
#include <ModelLoader.h>
#include <SpriteProxy.h>

#ifdef USE_DIFFICULTY
#include "DifficultySelectState.h"
#endif

LevelSelectState::LevelSelectState()
	: myGUIManager(nullptr)
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

LevelSelectState::~LevelSelectState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void LevelSelectState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myIsActiveState = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_level_select.xml", nullptr, nullptr, nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void LevelSelectState::EndState()
{

}

const eStateStatus LevelSelectState::Update(const float& aDeltaTime)
{
	aDeltaTime;

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
	{
		myIsActiveState = false;
		return eStateStatus::ePopMainState;
	}

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void LevelSelectState::Render()
{
	myGUIManager->Render();
	myLogo->Render(myLogoEndPosition);
	myLogoDust->Render(myLogoEndPosition, CU::Vector2<float>(1.f, 1.f), CU::Vector4<float>(1.f, 1.f, 1.f, 1.f));
}

void LevelSelectState::ResumeState()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void LevelSelectState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void LevelSelectState::ReceiveMessage(const OnClickMessage& aMessage)
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
			myStateStack->PushSubGameState(new DifficultySelectState(aMessage.myID));
#endif
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