#include "stdafx.h"

#include <Instance.h>
#include "Level.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>

Level::Level()
	: myEntities(512)
{
	myPlayer = new Player();
	myScene = new Prism::Scene(*myPlayer->GetCamera());
}

Level::~Level()
{
	myEntities.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
}

void Level::AddEntity(Entity* aEntity)
{
	myEntities.Add(aEntity);
}

void Level::Update(const float aDeltaTime)
{
	myPlayer->Update(aDeltaTime);
}

void Level::Render()
{
	myScene->Render();
}