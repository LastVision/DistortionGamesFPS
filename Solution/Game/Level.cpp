#include "stdafx.h"

#include <Instance.h>
#include "Level.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>

#include <PhysicsInterface.h>

Level::Level()
	: myEntities(512)
{
	Prism::PhysicsInterface::Create();
	myScene = new Prism::Scene();
	myPlayer = new Player(myScene);
	myScene->SetCamera(*myPlayer->GetCamera());

}

Level::~Level()
{
	myEntities.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	Prism::PhysicsInterface::Destroy();
}

void Level::AddEntity(Entity* aEntity)
{
	myEntities.Add(aEntity);
}

void Level::Update(const float aDeltaTime)
{
	static float timer = 0.f;
	timer += aDeltaTime;
	if (timer > 0.1f)
	{
		timer = 0.f;

		Spawn();
	}
	myPlayer->Update(aDeltaTime);
	for (int i = 0; i < myEntities.Size(); ++i)
	{
		myEntities[i]->Update(aDeltaTime);
	}
	Prism::PhysicsInterface::GetInstance()->Update();
}

void Level::Render()
{
	myScene->Render();
	myPlayer->Render();
}

void Level::Spawn()
{

}