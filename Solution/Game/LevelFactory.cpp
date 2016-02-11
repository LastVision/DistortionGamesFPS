#include "stdafx.h"

#include "AIDirector.h"
#include <Camera.h>
#include <CollisionComponent.h>
#include <CommonHelper.h>
#include "Console.h"
#include <DirectionalLight.h>
#include "dirent.h"
#include <Defines.h>
#include <Effect.h>
#include <EffectContainer.h>
#include <EmitterMessage.h>
#include <EngineEnums.h>
#include <EntityFactory.h>
#include <EntityEnumConverter.h>
#include "Level.h"
#include "LevelFactory.h"
#include <MathHelper.h>
#include <ModelLoader.h>
#include <NavMesh.h>
#include "NeutralDirector.h"
#include "PlayerDirector.h"
#include <PollingStation.h>
#include <PointLight.h>
#include <PostMaster.h>
#include "ScriptInterface.h"
#include <ScriptSystem.h>
#include <SpawnUnitMessage.h>
#include <SpotLight.h>
#include <TimerManager.h>
#include <Terrain.h>
#include <TextureContainer.h>
#include <Scene.h>
#include <XMLReader.h>


NavmeshCutBox::NavmeshCutBox(const CU::Vector3f& aPosition, const CU::Vector3f& aExtend, const CU::Vector3f& aRotation)
	: myPosition(aPosition)
	, myExtend(aExtend / 2.f)
	, myRotation(aRotation)
{
}

CU::GrowingArray<CU::Vector2<float>> NavmeshCutBox::GetCutMesh() const
{
	CU::GrowingArray<CU::Vector2<float>> points(4);

	CU::Vector2<float> pos(myPosition.x, myPosition.z);

	points.Add({ -myExtend.x, -myExtend.z });
	points.Add({ -myExtend.x, +myExtend.z });
	points.Add({ +myExtend.x, +myExtend.z });
	points.Add({ +myExtend.x, -myExtend.z });

	CU::Matrix33<float> rotationMatrix(CU::Matrix33<float>::CreateRotateAroundZ(-myRotation.y));

	for (int i = 0; i < points.Size(); ++i)
	{
		points[i] = points[i] * rotationMatrix;
		points[i] += pos;
	}
	return points;
}

LevelFactory::LevelFactory(const std::string& aLevelListPath, Prism::Camera& aCamera, GUI::Cursor* aCursor)
	: myCurrentLevel(nullptr)
	, myCurrentID(0)
	, myOldLevel(nullptr)
	, myIsLoading(false)
	, myLoadLevelThread(nullptr)
	, myDirectionalLights(4)
	, mySpotLights(4)
	, myPointLights(4)
	, myLevelPaths(2)
	, myCamera(aCamera)
	, myCursor(aCursor)
	, myCutBoxes(128)
	, myTerrain(nullptr)
{
	ReadLevelList(aLevelListPath);
}

LevelFactory::~LevelFactory()
{
	SAFE_DELETE(myLoadLevelThread);
	myDirectionalLights.DeleteAll();
}

Level* LevelFactory::LoadLevel(const int& aID, bool aPauseModelLoader, eDifficulty aDifficulty)
{
	if (myLevelPaths.find(aID) == myLevelPaths.end())
	{
		DL_ASSERT("[LevelFactory] Non-existing ID in LoadLevel! ID must correspond with levelList.xml");
	}
	myCurrentID = aID;

	return LoadCurrentLevel(aPauseModelLoader, aDifficulty);
}

Level* LevelFactory::LoadCurrentLevel(bool aPauseModelLoader, eDifficulty aDifficulty)
{
	myIsLoading = true;
	if (aPauseModelLoader == true)
	{
		Prism::ModelLoader::GetInstance()->Pause();
	}
	delete myCurrentLevel;
	myCurrentLevel = nullptr;

	LoadTerrain(myLevelPaths[myCurrentID]);
	myCurrentLevel = new Level(myCamera, myTerrain, myCursor, aDifficulty);

	SAFE_DELETE(myLoadLevelThread);
	//myLoadLevelThread = new std::thread(&LevelFactory::ReadLevel, this, myLevelPaths[myCurrentID]);
	std::string tutorialPath;
	ReadLevel(myLevelPaths[myCurrentID], tutorialPath);

	myCurrentLevel->myPlayer->InitGUI(myCurrentLevel->myAI, myCamera, myCurrentID);

	LUA::ScriptSystem::GetInstance()->CallFunction("Init", {});

	CU::Vector3f weatherPosition;
	weatherPosition.z = 136.f;
	weatherPosition.y = 10.f;
	weatherPosition.x = 126.f;
	if (globalUseWeather == true)
	{
		PostMaster::GetInstance()->SendMessage(EmitterMessage("weather_snow", weatherPosition, true));
		//PostMaster::GetInstance()->SendMessage(EmitterMessage("weather_snow", &myCamera, true));
	}

	myCurrentLevel->LoadTutorial(myCamera, tutorialPath); // needs to be after InitGUI

	if (aPauseModelLoader == true)
	{
		Prism::ModelLoader::GetInstance()->UnPause();
	}
	return myCurrentLevel;
}

Level* LevelFactory::LoadNextLevel(eDifficulty aDifficulty)
{
	if (IsLastLevel() == true)
	{
		return myCurrentLevel;
	}

	return LoadLevel(myCurrentID + 1, true, aDifficulty);
}

bool LevelFactory::IsLastLevel() const
{
	if (myLevelPaths.find(myCurrentID + 1) == myLevelPaths.end())
	{
		return true;
	}
	return false;
}

void LevelFactory::Cleanup()
{
	myLoadLevelThread->join();
	SAFE_DELETE(myLoadLevelThread);
}

void LevelFactory::ReadLevelList(const std::string& aLevelListPath)
{
	myLevelPaths.clear();
	XMLReader reader;
	reader.OpenDocument(aLevelListPath);
	std::string levelPath = "";
	int ID = -1;
	int lastID = ID - 1;

	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	for (tinyxml2::XMLElement* levelElement = reader.FindFirstChild(rootElement); levelElement != nullptr; levelElement = reader.FindNextElement(levelElement))
	{
		lastID = ID;

		reader.ForceReadAttribute(levelElement, "ID", ID);
		reader.ForceReadAttribute(levelElement, "path", levelPath);
		myLevelPaths[ID] = levelPath;

		if (ID - 1 != lastID)
		{
			DL_ASSERT("[LevelFactory] Wrong ID-number in levelList.xml! The numbers should be counting up, in order.");
		}
		if (myCurrentID >= 10)
		{
			DL_ASSERT("[LevelFactory] Can't handle level ID with two digits.");
		}
	}
	reader.CloseDocument();
}

void LevelFactory::ReadLevel(const std::string& aLevelPath, std::string& aTutorialPathOut)
{
	//Prism::ModelLoader* modelLoader = Prism::ModelLoader::GetInstance();
	Prism::EffectContainer* effectContainer = Prism::EffectContainer::GetInstance();
	//modelLoader->Pause();
	Prism::Engine::GetInstance()->myIsLoading = true;

	//FindTextures("Data/Resource/Texture/Particle/");

	myDirectionalLights.DeleteAll();
	myPointLights.DeleteAll();
	mySpotLights.DeleteAll();

	ReadLevelSetting(aLevelPath);

	XMLReader reader;
	reader.OpenDocument(aLevelPath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	tinyxml2::XMLElement* cameraElement = reader.ForceFindFirstChild(levelElement, "camera");

	CU::Vector3<float> cameraPosition(myCamera.GetOrientation().GetPos());
	reader.ForceReadAttribute(cameraElement, "x", cameraPosition.x);
	reader.ForceReadAttribute(cameraElement, "z", cameraPosition.z);

	myCamera.SetPosition(cameraPosition);

	std::string cubeMap;
	reader.ForceReadAttribute(reader.ForceFindFirstChild(levelElement, "cubemap"), "source", cubeMap);
	effectContainer->SetCubeMap(cubeMap);

	reader.ForceReadAttribute(reader.ForceFindFirstChild(levelElement, "tutorial"), "source", aTutorialPathOut);

	std::string luaPath;
	reader.ForceReadAttribute(reader.ForceFindFirstChild(levelElement, "lua"), "source", luaPath);
	LUA::ScriptSystem::Create();
	LUA::ScriptSystem::GetInstance()->Init(luaPath, ScriptInterface::RegisterFunctions);


	std::string aiPath;
	reader.ForceReadAttribute(reader.ForceFindFirstChild(levelElement, "ai"), "source", aiPath);
	myCurrentLevel->myAI->LoadAISettings(aiPath);

	Console::GetInstance(); // needs to be here to create console.

	LoadLights(reader, levelElement);
#ifndef USE_BINARY_TERRAIN
	LoadCutBoxes(reader, levelElement);
#endif
	LoadParticles(reader, levelElement);

	//modelLoader->UnPause();
	//modelLoader->WaitUntilFinished();

	effectContainer->GetEffect("Data/Resource/Shader/S_effect_pbl.fx")->SetAmbientHue(myAmbientHue);

#ifndef USE_BINARY_TERRAIN
	CU::TimerManager::GetInstance()->StartTimer("CreateNavMesh");
	myTerrain->CreateNavMesh();
	for (int i = 0; i < myCutBoxes.Size(); ++i)
	{
		myTerrain->GetNavMesh()->Cut(myCutBoxes[i]->GetCutMesh());
	}
	for (int i = 0; i < myCurrentLevel->myEntities.Size(); ++i)
	{
		//DL_ASSERT("Cutting from entities not supported.");
		myTerrain->GetNavMesh()->Cut(myCurrentLevel->myEntities[i]->GetCutMesh());
	}
	myTerrain->GetNavMesh()->CalcHeights(myTerrain);
	int elapsed = static_cast<int>(
		CU::TimerManager::GetInstance()->StopTimer("CreateNavMesh").GetMilliseconds());
	RESOURCE_LOG("Creating NavMesh took %d ms", elapsed);
	myCutBoxes.DeleteAll();
#endif

	myTerrain->CreatePathFinder();

	LoadBases(reader, levelElement);
#ifdef LOAD_PROPS
	LoadProps(reader, levelElement);
#endif
	LoadControlPoints(reader, levelElement);
	LoadUnits(reader, levelElement);
	LoadArtifacts(reader, levelElement);
	reader.CloseDocument();


	Prism::Engine::GetInstance()->myIsLoading = false;

	myIsLoading = false;
}

void LevelFactory::ReadLevelSetting(const std::string& aLevelPath)
{
	XMLReader reader;
	int dotPos = aLevelPath.find_first_of(".");
	std::string settingPath = aLevelPath.substr(0, dotPos) + "_settings.xml";
	reader.OpenDocument(settingPath);
	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	tinyxml2::XMLElement* ambientElement = reader.ForceFindFirstChild(rootElement, "Ambient");
	myAmbientHue = { 0.f, 0.f, 0.f, 0.f };
	reader.ForceReadAttribute(ambientElement, "r", myAmbientHue.x);
	reader.ForceReadAttribute(ambientElement, "g", myAmbientHue.y);
	reader.ForceReadAttribute(ambientElement, "b", myAmbientHue.z);

	myAmbientHue.x = myAmbientHue.x / 255.f;
	myAmbientHue.y = myAmbientHue.y / 255.f;
	myAmbientHue.z = myAmbientHue.z / 255.f;

	int maxVictoryPoint = -1;
	tinyxml2::XMLElement* victoryElement = reader.ForceFindFirstChild(rootElement, "MaxVictoryPoint");
	reader.ForceReadAttribute(victoryElement, "value", maxVictoryPoint);
	myCurrentLevel->myMaxVictoryPoint = maxVictoryPoint;

	int playerGunpowder = 0;
	int aiGunpowder = 0;

	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "gunpowder"), "player", playerGunpowder);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "gunpowder"), "ai", aiGunpowder);

	myCurrentLevel->myPlayer->SetGunPowder(playerGunpowder);
	myCurrentLevel->myAI->SetGunPowder(aiGunpowder);


	int playerBaseincome = 0;
	int aiBaseincome = 0;

	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "baseincome"), "player", playerBaseincome);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "baseincome"), "ai", aiBaseincome);

	myCurrentLevel->myPlayer->SetBaseIncome(playerBaseincome);
	myCurrentLevel->myAI->SetBaseIncome(aiBaseincome);

	reader.CloseDocument();
}

void LevelFactory::LoadLights(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	LoadDirectionalLights(aReader, aLevelElement);
}

void LevelFactory::LoadDirectionalLights(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aLevelElement, "directionallight"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "directionallight"))
	{
		tinyxml2::XMLElement* directionalElement = aReader.ForceFindFirstChild(entityElement, "rotation");

		Prism::DirectionalLight* newDirLight = new Prism::DirectionalLight();

		CU::Vector3<float> lightDirection;
		aReader.ForceReadAttribute(directionalElement, "X", lightDirection.x);
		aReader.ForceReadAttribute(directionalElement, "Y", lightDirection.y);
		aReader.ForceReadAttribute(directionalElement, "Z", lightDirection.z);

		CU::Matrix44<float> orientation;

		CU::GetOrientation(orientation, lightDirection);
		CU::Vector3<float> direction(0.f, 0.f, 1.f);

		direction = direction * orientation;

		newDirLight->SetDir(direction);

		directionalElement = aReader.ForceFindFirstChild(entityElement, "color");

		CU::Vector4<float> lightColor;
		aReader.ForceReadAttribute(directionalElement, "R", lightColor.x);
		aReader.ForceReadAttribute(directionalElement, "G", lightColor.y);
		aReader.ForceReadAttribute(directionalElement, "B", lightColor.z);
		aReader.ForceReadAttribute(directionalElement, "A", lightColor.w);
		newDirLight->SetColor(lightColor);

		myDirectionalLights.Add(newDirLight);
	}

	for (int i = 0; i < myDirectionalLights.Size(); ++i)
	{
		myCurrentLevel->myScene->AddLight(myDirectionalLights[i]);
	}
}

void LevelFactory::LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aLevelElement, "prop"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "prop"))
	{
		std::string propType;
		aReader.ForceReadAttribute(entityElement, "propType", propType);
		propType = CU::ToLower(propType);
		tinyxml2::XMLElement* propElement = aReader.ForceFindFirstChild(entityElement, "position");
		CU::Vector3<float> propPosition;
		aReader.ForceReadAttribute(propElement, "X", propPosition.x);
		aReader.ForceReadAttribute(propElement, "Y", propPosition.y);
		aReader.ForceReadAttribute(propElement, "Z", propPosition.z);

#ifndef _DEBUG
		DL_ASSERT_EXP(propPosition.x >= 0.f && propPosition.z >= 0.f, CU::Concatenate("Found %s outside Map", propType.c_str()));
#endif

		propElement = aReader.ForceFindFirstChild(entityElement, "rotation");
		CU::Vector3<float> propRotation;
		aReader.ForceReadAttribute(propElement, "X", propRotation.x);
		aReader.ForceReadAttribute(propElement, "Y", propRotation.y);
		aReader.ForceReadAttribute(propElement, "Z", propRotation.z);

		propElement = aReader.ForceFindFirstChild(entityElement, "scale");
		CU::Vector3<float> propScale;
		aReader.ForceReadAttribute(propElement, "X", propScale.x);
		aReader.ForceReadAttribute(propElement, "Y", propScale.y);
		aReader.ForceReadAttribute(propElement, "Z", propScale.z);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);

		myCurrentLevel->myEntities.Add(EntityFactory::CreateEntity(eOwnerType::NEUTRAL, eEntityType::PROP, propType,
			Prism::eOctreeType::STATIC, *myCurrentLevel->myScene, propPosition, *myCurrentLevel->myTerrain,
			propRotation, propScale));
		myCurrentLevel->myEntities.GetLast()->AddToScene();
		myCurrentLevel->myEntities.GetLast()->Reset();
		if (myCurrentLevel->myEntities.GetLast()->GetComponent<CollisionComponent>() != nullptr)
		{
			PollingStation::GetInstance()->RegisterEntity(myCurrentLevel->myEntities.GetLast());
		}
	}
}

void LevelFactory::LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aLevelElement, "unit"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "unit"))
	{
		std::string unitType;
		aReader.ForceReadAttribute(entityElement, "type", unitType);
		std::string owner;
		aReader.ForceReadAttribute(entityElement, "owner", owner);
		unitType = CU::ToLower(unitType);
		tinyxml2::XMLElement* element = aReader.ForceFindFirstChild(entityElement, "position");
		CU::Vector3<float> creepPosition;
		aReader.ForceReadAttribute(element, "X", creepPosition.x);
		aReader.ForceReadAttribute(element, "Y", creepPosition.y);
		aReader.ForceReadAttribute(element, "Z", creepPosition.z);

		element = aReader.ForceFindFirstChild(entityElement, "rotation");
		CU::Vector3<float> creepRotation;
		aReader.ForceReadAttribute(element, "X", creepRotation.x);
		aReader.ForceReadAttribute(element, "Y", creepRotation.y);
		aReader.ForceReadAttribute(element, "Z", creepRotation.z);

		element = aReader.ForceFindFirstChild(entityElement, "scale");
		CU::Vector3<float> creepScale;
		aReader.ForceReadAttribute(element, "X", creepScale.x);
		aReader.ForceReadAttribute(element, "Y", creepScale.y);
		aReader.ForceReadAttribute(element, "Z", creepScale.z);

		creepRotation.x = CU::Math::DegreeToRad(creepRotation.x);
		creepRotation.y = CU::Math::DegreeToRad(creepRotation.y);
		creepRotation.z = CU::Math::DegreeToRad(creepRotation.z);

		PostMaster::GetInstance()->SendMessage(SpawnUnitMessage(EntityEnumConverter::ConvertStringToUnitType(unitType)
			, EntityEnumConverter::ConvertStringToOwnerType(owner)
			, { creepPosition.x, creepPosition.z }
		, { creepPosition.x, creepPosition.z }));
	}
}

void LevelFactory::LoadControlPoints(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aLevelElement, "controlPoint"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "controlPoint"))
	{
		std::string controlPointType;
		aReader.ForceReadAttribute(entityElement, "type", controlPointType);
		controlPointType = CU::ToLower(controlPointType);
		tinyxml2::XMLElement* controlPointElement = aReader.ForceFindFirstChild(entityElement, "position");
		CU::Vector3<float> propPosition;
		aReader.ForceReadAttribute(controlPointElement, "X", propPosition.x);
		aReader.ForceReadAttribute(controlPointElement, "Y", propPosition.y);
		aReader.ForceReadAttribute(controlPointElement, "Z", propPosition.z);

		controlPointElement = aReader.ForceFindFirstChild(entityElement, "rotation");
		CU::Vector3<float> propRotation;
		aReader.ForceReadAttribute(controlPointElement, "X", propRotation.x);
		aReader.ForceReadAttribute(controlPointElement, "Y", propRotation.y);
		aReader.ForceReadAttribute(controlPointElement, "Z", propRotation.z);

		controlPointElement = aReader.ForceFindFirstChild(entityElement, "scale");
		CU::Vector3<float> propScale;
		aReader.ForceReadAttribute(controlPointElement, "X", propScale.x);
		aReader.ForceReadAttribute(controlPointElement, "Y", propScale.y);
		aReader.ForceReadAttribute(controlPointElement, "Z", propScale.z);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);

		myCurrentLevel->myEntities.Add(EntityFactory::CreateEntity(eOwnerType::NEUTRAL,
			EntityEnumConverter::ConvertStringToEntityType(controlPointType), Prism::eOctreeType::STATIC,
			*myCurrentLevel->myScene, propPosition, *myCurrentLevel->myTerrain, propRotation, propScale));
		myCurrentLevel->myEntities.GetLast()->AddToScene();
		myCurrentLevel->myEntities.GetLast()->Reset();
	}
}

void LevelFactory::LoadBases(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	int enemyBase = 0;
	int playerBase = 0;
	for (tinyxml2::XMLElement* baseElement = aReader.FindFirstChild(aLevelElement); baseElement != nullptr;
		baseElement = aReader.FindNextElement(baseElement))
	{
		std::string elementName = CU::ToLower(baseElement->Name());
		if (elementName == "enemybase" || elementName == "playerbase")
		{
			std::string baseType;
			aReader.ForceReadAttribute(baseElement, "baseType", baseType);
			baseType = CU::ToLower(baseType);

			tinyxml2::XMLElement* propElement = aReader.ForceFindFirstChild(baseElement, "position");
			CU::Vector3<float> propPosition;
			aReader.ForceReadAttribute(propElement, "X", propPosition.x);
			aReader.ForceReadAttribute(propElement, "Y", propPosition.y);
			aReader.ForceReadAttribute(propElement, "Z", propPosition.z);

			propElement = aReader.ForceFindFirstChild(baseElement, "rotation");
			CU::Vector3<float> propRotation;
			aReader.ForceReadAttribute(propElement, "X", propRotation.x);
			aReader.ForceReadAttribute(propElement, "Y", propRotation.y);
			aReader.ForceReadAttribute(propElement, "Z", propRotation.z);

			propElement = aReader.ForceFindFirstChild(baseElement, "scale");
			CU::Vector3<float> propScale;
			aReader.ForceReadAttribute(propElement, "X", propScale.x);
			aReader.ForceReadAttribute(propElement, "Y", propScale.y);
			aReader.ForceReadAttribute(propElement, "Z", propScale.z);

			propRotation.x = CU::Math::DegreeToRad(propRotation.x);
			propRotation.y = CU::Math::DegreeToRad(propRotation.y);
			propRotation.z = CU::Math::DegreeToRad(propRotation.z);

			if (elementName == "enemybase")
			{
				myCurrentLevel->myAI->myBuilding = EntityFactory::CreateEntity(eOwnerType::ENEMY,
					EntityEnumConverter::ConvertStringToEntityType(baseType), Prism::eOctreeType::STATIC,
					*myCurrentLevel->myScene, propPosition, *myCurrentLevel->myTerrain, propRotation, propScale);
				myCurrentLevel->myAI->myBuilding->AddToScene();
				myCurrentLevel->myAI->myBuilding->Reset();
				PollingStation::GetInstance()->RegisterEntity(myCurrentLevel->myAI->myBuilding);
				enemyBase++;
			}
			else if (elementName == "playerbase")
			{
				myCurrentLevel->myPlayer->myBuilding = EntityFactory::CreateEntity(eOwnerType::PLAYER,
					EntityEnumConverter::ConvertStringToEntityType(baseType), Prism::eOctreeType::STATIC,
					*myCurrentLevel->myScene, propPosition, *myCurrentLevel->myTerrain, propRotation, propScale);
				myCurrentLevel->myPlayer->myBuilding->AddToScene();
				myCurrentLevel->myPlayer->myBuilding->Reset();
				PollingStation::GetInstance()->RegisterEntity(myCurrentLevel->myPlayer->myBuilding);
				playerBase++;
			}
		}
	}
	//DL_ASSERT_EXP(enemyBase <= 1, "Enemy can't have more than one base.");
	//DL_ASSERT_EXP(enemyBase >= 1, "Enemy can't have less than one base.");
	//DL_ASSERT_EXP(playerBase <= 1, "Player can't have more than one base.");
	//DL_ASSERT_EXP(playerBase >= 1, "Player can't have less than one base.");

}

void LevelFactory::LoadCutBoxes(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	for (tinyxml2::XMLElement* e = aReader.FindFirstChild(aLevelElement); e != nullptr;
		e = aReader.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == "navmeshcutbox")
		{
			tinyxml2::XMLElement* boxElement = aReader.ForceFindFirstChild(e, "position");
			CU::Vector3<float> boxPosition;
			aReader.ForceReadAttribute(boxElement, "X", boxPosition.x);
			aReader.ForceReadAttribute(boxElement, "Y", boxPosition.y);
			aReader.ForceReadAttribute(boxElement, "Z", boxPosition.z);

			boxElement = aReader.ForceFindFirstChild(e, "rotation");
			CU::Vector3<float> boxRotation;
			aReader.ForceReadAttribute(boxElement, "X", boxRotation.x);
			aReader.ForceReadAttribute(boxElement, "Y", boxRotation.y);
			aReader.ForceReadAttribute(boxElement, "Z", boxRotation.z);

			boxElement = aReader.ForceFindFirstChild(e, "scale");
			CU::Vector3<float> boxScale;
			aReader.ForceReadAttribute(boxElement, "X", boxScale.x);
			aReader.ForceReadAttribute(boxElement, "Y", boxScale.y);
			aReader.ForceReadAttribute(boxElement, "Z", boxScale.z);

			boxRotation.x = CU::Math::DegreeToRad(boxRotation.x);
			boxRotation.y = CU::Math::DegreeToRad(boxRotation.y);
			boxRotation.z = CU::Math::DegreeToRad(boxRotation.z);

			NavmeshCutBox* newCutBox = new NavmeshCutBox(boxPosition, boxScale, boxRotation);
			myCutBoxes.Add(newCutBox);
		}
	}
}

void LevelFactory::LoadArtifacts(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aLevelElement, "artifact"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "artifact"))
	{
		std::string propType;
		aReader.ForceReadAttribute(entityElement, "type", propType);
		propType = CU::ToLower(propType);

		tinyxml2::XMLElement* propElement = aReader.ForceFindFirstChild(entityElement, "position");
		CU::Vector3<float> propPosition;
		aReader.ForceReadAttribute(propElement, "X", propPosition.x);
		aReader.ForceReadAttribute(propElement, "Y", propPosition.y);
		aReader.ForceReadAttribute(propElement, "Z", propPosition.z);

		propElement = aReader.ForceFindFirstChild(entityElement, "rotation");
		CU::Vector3<float> propRotation;
		aReader.ForceReadAttribute(propElement, "X", propRotation.x);
		aReader.ForceReadAttribute(propElement, "Y", propRotation.y);
		aReader.ForceReadAttribute(propElement, "Z", propRotation.z);

		propElement = aReader.ForceFindFirstChild(entityElement, "scale");
		CU::Vector3<float> propScale;
		aReader.ForceReadAttribute(propElement, "X", propScale.x);
		aReader.ForceReadAttribute(propElement, "Y", propScale.y);
		aReader.ForceReadAttribute(propElement, "Z", propScale.z);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);

		myCurrentLevel->myEntities.Add(EntityFactory::CreateEntity(eOwnerType::NEUTRAL, eEntityType::ARTIFACT, propType,
			Prism::eOctreeType::STATIC, *myCurrentLevel->myScene, propPosition, *myCurrentLevel->myTerrain,
			propRotation, propScale));
		myCurrentLevel->myEntities.GetLast()->Reset();
		myCurrentLevel->myEntities.GetLast()->AddToScene();

		PollingStation::GetInstance()->AddArtifact(myCurrentLevel->myEntities.GetLast());
		PostMaster::GetInstance()->SendMessage(EmitterMessage("artifact_glow", true
			, myCurrentLevel->myEntities.GetLast()->GetId()));
	}
}

void LevelFactory::LoadTerrain(const std::string& aLevelPath)
{
	CU::TimerManager::GetInstance()->StartTimer("LoadTerrain");

	XMLReader reader;
	reader.OpenDocument(aLevelPath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");
	std::string heightMap;
	std::string texturePath;
	std::string icePath;
	std::string normalPath;

	tinyxml2::XMLElement* terrainElement = reader.FindFirstChild(levelElement, "terrain");
	reader.ForceReadAttribute(terrainElement, "heightmap", heightMap);
	reader.ForceReadAttribute(terrainElement, "texture", texturePath);
	reader.ForceReadAttribute(terrainElement, "normalmap", normalPath);

	tinyxml2::XMLElement* iceElement = reader.FindFirstChild(levelElement, "ice");
	reader.ForceReadAttribute(iceElement, "texture", icePath);

	reader.CloseDocument();

#ifdef USE_BINARY_TERRAIN
	myTerrain = new Prism::Terrain(CU::GetGeneratedDataFolderFilePath(aLevelPath, "nav"), texturePath, normalPath, icePath);
#else
	myTerrain = new Prism::Terrain(heightMap, texturePath, normalPath, { 256.f, 256.f }, 10.f, CU::Matrix44<float>(), icePath);
#endif

	int elapsed = static_cast<int>(
		CU::TimerManager::GetInstance()->StopTimer("LoadTerrain").GetMilliseconds());
	RESOURCE_LOG("Loading Terrain took %d ms", elapsed);
}

void LevelFactory::LoadParticles(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aLevelElement, "particle"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "particle"))
	{
		std::string particleType;
		aReader.ForceReadAttribute(entityElement, "type", particleType);
		particleType = CU::ToLower(particleType);

		tinyxml2::XMLElement* propElement = aReader.ForceFindFirstChild(entityElement, "position");
		CU::Vector3<float> propPosition;
		aReader.ForceReadAttribute(propElement, "X", propPosition.x);
		aReader.ForceReadAttribute(propElement, "Y", propPosition.y);
		aReader.ForceReadAttribute(propElement, "Z", propPosition.z);

		PostMaster::GetInstance()->SendMessage(EmitterMessage(particleType, propPosition, true));
	}
}

