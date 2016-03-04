#include "stdafx.h"
#include <Instance.h>
#include "ClientLevel.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include "NetworkMessageTypes.h"
#include "NetworkMessageTypes.h"
#include "NetMessagePosition.h"
#include "DebugDrawer.h"

#include "EntityFactory.h"
#include "Entity.h"
#include "GameEnum.h"
#include <NetMessageConnectMessage.h>
#include <PhysicsInterface.h>
#include "NetMessageOnJoin.h"

#include "ClientNetworkManager.h"
#include "DeferredRenderer.h"
#include <PointLight.h>

ClientLevel::ClientLevel()
	: myInstanceOrientations(16)
	, myInstances(16)
{
	Prism::PhysicsInterface::Create();
	//Prism::PhysicsInterface::Destroy();
	//Prism::PhysicsInterface::GetInstance()->RayCast({ 0, 0, 0 }, { 0, 1, 0 }, 10.f);
	//Prism::PhysicsInterface::GetInstance()->Update();

	myScene = new Prism::Scene();
	myPlayer = new Player(myScene);
	myScene->SetCamera(*myPlayer->GetCamera());

	/*Prism::PointLight* light = new Prism::PointLight();
	light->SetPosition({ 0.f, 1.f, 0.f, 0.f });
	light->SetColor({ 0.f, 1.f, 0.f, 1.f });
	light->SetRange(50.f);
	myScene->AddLight(light);*/

	//myTempPosition = { 835.f, 0.f, -1000.f };

	//myDeferredRenderer = new Prism::DeferredRenderer();

	CU::Matrix44f orientation;
	myInstanceOrientations.Add(orientation);

	//Prism::Instance* newInstance;
	//newInstance = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Prop/SM_dev_box.fbx", "Data/Resource/Shader/S_effect_pbldebug.fx"), myInstanceOrientations[0]);
	//myInstances.Add(newInstance);
	//myScene->AddInstance(myInstances[0], true);
}

ClientLevel::~ClientLevel()
{
	myInstances.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	//SAFE_DELETE(myDeferredRenderer);
	Prism::PhysicsInterface::Destroy();
}

void ClientLevel::Update(const float aDeltaTime)
{
	SharedLevel::Update(aDeltaTime);
	ClientNetworkManager::GetInstance()->Update(aDeltaTime);
	myPlayer->Update(aDeltaTime);

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_C) == true)
	{
		ClientNetworkManager::GetInstance()->ConnectToServer("81.170.227.192");
	}
	unsigned short ms = ClientNetworkManager::GetInstance()->GetResponsTime();
	float kbs = ClientNetworkManager::GetInstance()->GetDataSent();

	DEBUG_PRINT(ms);
	DEBUG_PRINT(kbs);
	Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetCamera());

	if (ClientNetworkManager::GetInstance()->GetClients().Size() > 0)
	{
		myInstanceOrientations[0].SetPos(ClientNetworkManager::GetInstance()->GetClients()[0].myPosition);
	}


	Prism::PhysicsInterface::GetInstance()->Update();
}

void ClientLevel::Render()
{
	//myDeferredRenderer->Render(myScene);
	myScene->Render();
	myPlayer->Render();



}