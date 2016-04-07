#include "stdafx.h"
#include <AudioInterface.h>
#include "CompleteGameState.h"
#include "CreditMenuState.h"
#include "GUIManager.h"
#include "InputWrapper.h"
#include "PostMaster.h"
#include "OnClickMessage.h"
#include "InGameState.h"
#include "HelpState.h"
#include <Text.h>
#include <WidgetContainer.h>
#include <FadeMessage.h>
#include "ClientNetworkManager.h"
#include <NetMessageDisconnect.h>

CompleteGameState::CompleteGameState()
	: myGUIManager(nullptr)
	, myWaitTimer(4.f)
{
	ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
}


CompleteGameState::~CompleteGameState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
}

void CompleteGameState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myCursor = aCursor;
	myIsActiveState = true;
	myIsLetThrough = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_complete_game.xml", nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();

	OnResize(windowSize.x, windowSize.y);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void CompleteGameState::EndState()
{
	myIsActiveState = false;
}

void CompleteGameState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
	bool notDone = true;
}

const eStateStatus CompleteGameState::Update(const float& aDeltaTime)
{
	myWaitTimer -= aDeltaTime;

	if (myWaitTimer < 0 &&
		(CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true
		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN) == true
		|| CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true
		|| CU::InputWrapper::GetInstance()->MouseDown(0) == true
		|| CU::InputWrapper::GetInstance()->MouseDown(1) == true
		|| CU::InputWrapper::GetInstance()->MouseDown(2) == true))
	{
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new CreditMenuState());
	}

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void CompleteGameState::Render()
{
	myGUIManager->Render();
}

void CompleteGameState::ResumeState()
{
	myIsActiveState = true;
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}
