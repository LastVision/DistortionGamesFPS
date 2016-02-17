#include "stdafx.h"

#include <Instance.h>
#include "Level.h"
#include <ModelLoader.h>
#include <PlayerComponent.h>
#include <Scene.h>


Level::Level()
{
	myPlayer = new PlayerComponent();
	myScene = new Prism::Scene(*myPlayer->myCamera);
	myInstance = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Modular_set/Dev_set/SM_dev_wall_corner_out_200_x_300.fbx", "Data/Resource/Shader/S_effect_pbl.fx")
		, myInstanceOrientation, Prism::eOctreeType::DYNAMIC, 15.f);
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