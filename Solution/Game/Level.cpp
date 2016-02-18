#include "stdafx.h"

#include <Instance.h>
#include "Level.h"
#include <ModelLoader.h>
#include <PlayerComponent.h>
#include <Scene.h>

Level::Level()
{
	myPlayer = new PlayerComponent();
	myScene = new Prism::Scene(*myPlayer->GetCamera());
	myInstance = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Test_temp/Extra att test cube_2.fbx", "Data/Resource/Shader/S_effect_pbldebug.fx")
		, myInstanceOrientation);
	myInstanceOrientation = CU::Matrix44<float>::CreateRotateAroundY(M_PI);
	myScene->AddInstance(myInstance);
}

Level::~Level()
{
}

void Level::Update(const float aDeltaTime)
{
	myPlayer->Update(aDeltaTime);
}

void Level::Render()
{
	myScene->Render();
}