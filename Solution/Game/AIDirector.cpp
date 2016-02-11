#include "stdafx.h"

#include <ActorComponent.h>
#include "AIDirector.h"
#include "BlockMap.h"
#include <BlockMapMessage.h>
#include <BuildingComponent.h>
#include <ControllerComponent.h>
#include "DecisionMap.h"
#include "DifferenceMap.h"
#include <Entity.h>
#include <EntityFactory.h>
#include <FuzzySet.h>
#include <GameConstants.h>
#include <KillUnitMessage.h>
#include "InfluenceMap.h"
#include <InputWrapper.h>
#include <PollingStation.h>
#include <PostMaster.h>
#include <SpawnUnitMessage.h>
#include "TensionMap.h"
#include <TimeMultiplierMessage.h>
#include <TriggerComponent.h>
#include "VulnerabilityMap.h"
#include <XMLReader.h>


AIDirector::AIMaps::AIMaps()
{
	myPlayerInfluenceMap = new InfluenceMap();
	myBlockMap = new BlockMap();

	myInfluenceRenderIndex = 0;
}

AIDirector::AIMaps::~AIMaps()
{
	SAFE_DELETE(myPlayerInfluenceMap);
	SAFE_DELETE(myBlockMap);
}

AIDirector::AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, eDifficulty aDifficulty)
	: Director(eOwnerType::ENEMY, aTerrain)
	, myPlayerHasStarted(false)
	, myIdleUnits(32)
	, myUnitsOnMission(32)
	, myActionQueue(8)
	, myDifficulty(aDifficulty)
{
	for (int i = 0; i < GC::enemyGruntCount; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, "grunt", Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}

	for (int i = 0; i < GC::enemyRangerCount; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, "ranger", Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}

	for (int i = 0; i < GC::enemyTankCount; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, "tank", Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}

	
	PostMaster::GetInstance()->Subscribe(eMessageType::TIME_MULTIPLIER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::BLOCK_MAP, this);

	myFuzzySet = new CU::FuzzySet(static_cast<int>(eFuzzyAI::_COUNT));

	myCurrentAction.myFuzzyAction = eFuzzyAI::DO_NOTHING;
	myCurrentAction.myIsDone = true;

	myDecisionTimer.myMinTime = 5.f;
	myDecisionTimer.myMaxTime = 20.f;
}

AIDirector::~AIDirector()
{
	SAFE_DELETE(myFuzzySet);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TIME_MULTIPLIER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::BLOCK_MAP, this);
}

void AIDirector::RenderMaps(const Prism::Camera& aCamera)
{
	switch (myMaps.myInfluenceRenderIndex)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		myMaps.myPlayerInfluenceMap->Render(aCamera);
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	case 9:
		myMaps.myBlockMap->Render(aCamera);
		break;
	default:
		DL_ASSERT("Invalid myInfluenceRenderIndex");
		break;
	}
}

void AIDirector::Update(float aDeltaTime)
{
	aDeltaTime *= myTimeMultiplier;

	Director::Update(aDeltaTime);

	if (myPlayerHasStarted == false)
	{
		const BuildingComponent* building = PollingStation::GetInstance()->GetBase(eOwnerType::PLAYER)->GetComponent<BuildingComponent>();
		myPlayerHasStarted = building->GetHasSpawnedAtLeastOnce();
		//myPlayerHasStarted = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER).Size() > 0;
		myTimeMultiplier = 1.f;
		return;
	}

	UpdateInfluences();
	UpdateUnitLists();

	myDecisionTimer.Update(aDeltaTime);
	if (UpdateCurrentAction() == true)
	{
		if (FuzzyDecisionDone() == true)
		{
			UpdateAdvisors();
			InterpretFuzzySet();
			myDecisionTimer.Reset();
		}

		StartNextAction();
	}

	
}

void AIDirector::ReceiveMessage(const SpawnUnitMessage& aMessage)
{
	if (static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::RANGER && myHasUnlockedRanger == false) return;
	if (static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::TANK && myHasUnlockedTank == false) return;
	Director::ReceiveMessage(aMessage);
}

void AIDirector::ReceiveMessage(const TimeMultiplierMessage& aMessage)
{
	if (aMessage.myOwner == eOwnerType::ENEMY)
	{
		myTimeMultiplier = aMessage.myMultiplier;
		if (myTimeMultiplier > 0.f)
		{
			myPlayerHasStarted = true;
		}
	}
}

void AIDirector::ReceiveMessage(const BlockMapMessage& aMessage)
{
	if (aMessage.myShouldAddValue == true)
	{
		myMaps.myBlockMap->AddValue(1.f, 5.f, aMessage.myPosition);
	}
	else
	{
		myMaps.myBlockMap->RemoveValue(1.f, 5.f, aMessage.myPosition);
	}
}

void AIDirector::ReceiveMessage(const KillUnitMessage& aMessage)
{
	Director::ReceiveMessage(aMessage);

	if (aMessage.myOwnerType == myOwner)
	{
		eUnitType unitType = static_cast<eUnitType>(aMessage.myUnitType);
		switch (unitType)
		{
		case GRUNT:
			++myUpgradeData.myGruntKillCount;
			break;
		case RANGER:
			++myUpgradeData.myRangerKillCount;
			break;
		case TANK:
			++myUpgradeData.myTankKillCount;
			break;
		}

		ReceiveMessage(BlockMapMessage(aMessage.myTargetPosition, false));
	}
}

void AIDirector::LoadAISettings(const std::string& aFilePath)
{
	XMLReader reader;
	reader.OpenDocument(aFilePath);

	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");
	tinyxml2::XMLElement* controlPointAdvisor = reader.ForceFindFirstChild(root, "ControlPointAdvisor");
	
	tinyxml2::XMLElement* resourcePoint = reader.ForceFindFirstChild(controlPointAdvisor, "ResourcePoint");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(resourcePoint, "squadsize")
		, "value", myResourceData.myResourceSquadSize);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(resourcePoint, "optimalgunpowder")
		, "value", myResourceData.myOptimalGunpowerCount);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(resourcePoint, "gunpowdervalue")
		, "value", myResourceData.myGunpowderValue);

	tinyxml2::XMLElement* victoryPoint = reader.ForceFindFirstChild(controlPointAdvisor, "VictoryPoint");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(victoryPoint, "squadsize")
		, "value", myResourceData.myVictorySquadSize);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(victoryPoint, "optimalowningpercentage")
		, "value", myResourceData.myOptimalVictoryPoints);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(victoryPoint, "victorypointvalue")
		, "value", myResourceData.myVictoryPointValue);

	myResourceData.myOptimalVictoryPoints /= 100.f;



	tinyxml2::XMLElement* upgradeAdvisor = reader.ForceFindFirstChild(root, "UpgradeAdvisor");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(upgradeAdvisor, "gruntskilledbeforeupgrade")
		, "value", myUpgradeData.myGruntsKilledBeforeUpgrade);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(upgradeAdvisor, "rangerskilledbeforeupgrade")
		, "value", myUpgradeData.myRangersKilledBeforeUpgrade);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(upgradeAdvisor, "tankskilledbeforeupgrade")
		, "value", myUpgradeData.myTanksKilledBeforeUpgrade);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(upgradeAdvisor, "upgradevalue")
		, "value", myUpgradeData.myUpgradeValue);

	myUpgradeData.myGruntKillCount = 0;
	myUpgradeData.myRangerKillCount = 0;
	myUpgradeData.myTankKillCount = 0;


	tinyxml2::XMLElement* spawnAdvisor = reader.ForceFindFirstChild(root, "SpawnAdvisor");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(spawnAdvisor, "gruntvalue")
		, "value", mySpawnData.myGruntValue);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(spawnAdvisor, "rangervalue")
		, "value", mySpawnData.myRangerValue);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(spawnAdvisor, "tankvalue")
		, "value", mySpawnData.myTankValue);


	tinyxml2::XMLElement* decisionTime = reader.ForceFindFirstChild(root, "DecisionTime");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(decisionTime, "min")
		, "value", myDecisionTimer.myMinTime);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(decisionTime, "max")
		, "value", myDecisionTimer.myMaxTime);



	tinyxml2::XMLElement* difficultyElement = reader.ForceFindFirstChild(root, "Difficulty");
	float difficultyModifier = 1.f;
	switch (myDifficulty)
	{
	case eDifficulty::EASY:
		reader.ForceReadAttribute(reader.ForceFindFirstChild(difficultyElement, "easy")
			, "value", difficultyModifier);
		break;
	case eDifficulty::NORMAL:
		reader.ForceReadAttribute(reader.ForceFindFirstChild(difficultyElement, "normal")
			, "value", difficultyModifier);
		break;
	case eDifficulty::HARD:
		reader.ForceReadAttribute(reader.ForceFindFirstChild(difficultyElement, "hard")
			, "value", difficultyModifier);
		break;
	default:
		DL_ASSERT("Invalid AI-Difficulty");
		break;
	}

	myDecisionTimer.myMinTime *= difficultyModifier;
	myDecisionTimer.myMaxTime *= difficultyModifier;

	reader.CloseDocument();
}

 InfluenceMap* AIDirector::GetPlayerInfluenceMap()
{
	return myMaps.myPlayerInfluenceMap;
}

void AIDirector::UpdateInfluences()
{
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();

	if (input->KeyIsPressed(DIK_LSHIFT) || input->KeyIsPressed(DIK_RSHIFT))
	{
		if (input->KeyDown(DIK_F5))
		{
			myMaps.myInfluenceRenderIndex = 5;
		}
		else if (input->KeyDown(DIK_F6))
		{
			myMaps.myInfluenceRenderIndex = 6;
		}
		else if (input->KeyDown(DIK_F7))
		{
			myMaps.myInfluenceRenderIndex = 7;
		}
		else if (input->KeyDown(DIK_F8))
		{
			myMaps.myInfluenceRenderIndex = 8;
		}
		else if (input->KeyDown(DIK_F9))
		{
			myMaps.myInfluenceRenderIndex = 9;
		}
	}
	else
	{
		if (input->KeyDown(DIK_F5))
		{
			myMaps.myInfluenceRenderIndex = 0;
		}
		else if (input->KeyDown(DIK_F6))
		{
			myMaps.myInfluenceRenderIndex = 1;
		}
		else if (input->KeyDown(DIK_F7))
		{
			myMaps.myInfluenceRenderIndex = 2;
		}
		else if (input->KeyDown(DIK_F8))
		{
			myMaps.myInfluenceRenderIndex = 3;
		}
		else if (input->KeyDown(DIK_F9))
		{
			myMaps.myInfluenceRenderIndex = 4;
		}
	}

	myMaps.myPlayerInfluenceMap->Update();
	const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
	for (int i = 0; i < playerUnits.Size(); ++i)
	{
		myMaps.myPlayerInfluenceMap->AddValue(20.f, 30.f, playerUnits[i]->GetPosition());
	}


}

void AIDirector::UpdateUnitLists()
{
	myUnitsOnMission.RemoveAll();
	myIdleUnits.RemoveAll();

	for (int i = myActiveUnits.Size()-1; i >= 0; --i)
	{
		bool idle = true;

		const CU::GrowingArray<Entity*>& points = PollingStation::GetInstance()->GetVictoryAndResourcePoints();
		for (int j = 0; j < points.Size(); ++j)
		{
			TriggerComponent* trigger = points[j]->GetComponent<TriggerComponent>();
			if (trigger->GetOwnerGainingPoint() != myOwner)
			{
				float radius2 = trigger->GetRadiusSquared();
				float dist2 = CU::Length2(points[j]->GetPosition() - myActiveUnits[i]->GetPosition());
				if (dist2 < radius2)
				{
					idle = false;
				}
			}
		}


		bool ready = myActiveUnits[i]->GetComponent<ControllerComponent>()->IsReady();
		idle = idle && myActiveUnits[i]->GetState() == eEntityState::IDLE;
		if (idle == true && ready == true)
		{
			myIdleUnits.Add(myActiveUnits[i]);
		}
		else
		{
			myUnitsOnMission.Add(myActiveUnits[i]);
		}
	}

	for (int i = myUpgradeData.myArtifactHunters.Size() - 1; i >= 0; --i)
	{
		Entity* unit = myUpgradeData.myArtifactHunters[i];
		bool ready = unit->GetComponent<ControllerComponent>()->IsReady();
		bool idle = unit->GetState() == eEntityState::IDLE;
		if (idle == true && ready == true || unit->GetAlive() == false)
		{
			myUpgradeData.myArtifactHunters.RemoveCyclicAtIndex(i);
		}
	}
}


void AIDirector::UpdateAdvisors()
{
	myFuzzySet->Reset();

	*myFuzzySet += UpdateResourceAdvisor();
	*myFuzzySet += UpdateUpgradeAdvisor();
	*myFuzzySet += UpdateSpawningAdvisor();
	*myFuzzySet += UpdateRoamingAdvisor();

	myFuzzySet->Normalize();
}

CU::FuzzySet AIDirector::UpdateResourceAdvisor()
{
	float threshHold = 1.f - (static_cast<float>(myGunpowder) / static_cast<float>(myResourceData.myOptimalGunpowerCount));

	int ownedPointsCount = PollingStation::GetInstance()->GetResourcePointsCount(myOwner);
	int totalPointsCount = PollingStation::GetInstance()->GetResourcePoints().Size();
	float pointModifier = 1.f - (static_cast<float>(ownedPointsCount) / static_cast<float>(totalPointsCount));

	float fuzzyValue = threshHold * myResourceData.myGunpowderValue;
	fuzzyValue *= pointModifier;

	CU::FuzzySet set(static_cast<int>(eFuzzyAI::_COUNT));
	set.AddValue(static_cast<int>(eFuzzyAI::TAKE_RESOURCE_POINT), fuzzyValue);


	int ownedVictoryPointsCount = PollingStation::GetInstance()->GetVictoryPointsCount(myOwner);
	int totalVictoryPointsCount = PollingStation::GetInstance()->GetVictoryPoints().Size();
	float ownedVictoryPoints = (static_cast<float>(ownedVictoryPointsCount) / static_cast<float>(totalVictoryPointsCount));

	float victoryPointModifier = 1.f - ownedVictoryPoints / myResourceData.myOptimalVictoryPoints;
	fuzzyValue = victoryPointModifier * myResourceData.myVictoryPointValue;
	set.AddValue(static_cast<int>(eFuzzyAI::TAKE_VICTORY_POINT), fuzzyValue);

	return set;
}

CU::FuzzySet AIDirector::UpdateUpgradeAdvisor()
{
	CU::FuzzySet set(static_cast<int>(eFuzzyAI::_COUNT));
	BuildingComponent* building = myBuilding->GetComponent<BuildingComponent>();

	int mapArtifactCount = PollingStation::GetInstance()->GetArtifacts().Size();
	float artifactModifier = 0.f;
	if (mapArtifactCount > 0)
	{
		artifactModifier = 1.f;
	}

	//GRUNT
	float gruntValue = static_cast<float>(myUpgradeData.myGruntKillCount) / static_cast<float>(myUpgradeData.myGruntsKilledBeforeUpgrade);
	gruntValue = gruntValue * myUpgradeData.myUpgradeValue * artifactModifier;

	if (building->CanUpgrade(eUnitType::GRUNT) == false)
	{
		gruntValue = 0.f;
	}
	set.AddValue(static_cast<int>(eFuzzyAI::UPGRADE_GRUNT), gruntValue);

	//RANGER
	float rangerValue = static_cast<float>(myUpgradeData.myRangerKillCount) / static_cast<float>(myUpgradeData.myRangersKilledBeforeUpgrade);
	rangerValue = rangerValue * myUpgradeData.myUpgradeValue * artifactModifier;

	if (building->CanUpgrade(eUnitType::RANGER) == false)
	{
		rangerValue = 0.f;
	}
	set.AddValue(static_cast<int>(eFuzzyAI::UPGRADE_RANGER), rangerValue);

	//TANK
	float tankValue = static_cast<float>(myUpgradeData.myTankKillCount) / static_cast<float>(myUpgradeData.myTanksKilledBeforeUpgrade);
	tankValue = tankValue * myUpgradeData.myUpgradeValue * artifactModifier;

	if (building->CanUpgrade(eUnitType::TANK) == false)
	{
		tankValue = 0.f;
	}
	set.AddValue(static_cast<int>(eFuzzyAI::UPGRADE_TANK), tankValue);

	return set;
}

CU::FuzzySet AIDirector::UpdateSpawningAdvisor()
{
	CU::FuzzySet set(static_cast<int>(eFuzzyAI::_COUNT));

	float gruntCount = 0;
	float rangerCount = 0;
	float tankCount = 0;
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		if (myActiveUnits[i]->GetUnitType() == eUnitType::GRUNT)
		{
			++gruntCount;
		}
		else if (myActiveUnits[i]->GetUnitType() == eUnitType::RANGER)
		{
			++rangerCount;
		}
		else if (myActiveUnits[i]->GetUnitType() == eUnitType::TANK)
		{
			++tankCount;
		}
	}


	float gruntValue = 0.f;
	float rangerValue = 0.f;
	float tankValue = 0.f;
	if (myActiveUnits.Size() > 0)
	{
		gruntValue = (1 - (gruntCount / myActiveUnits.Size())) * mySpawnData.myGruntValue;
		rangerValue = (1 - (rangerCount / myActiveUnits.Size())) * mySpawnData.myRangerValue;
		tankValue = (1 - (tankCount / myActiveUnits.Size())) * mySpawnData.myTankValue;
	}

	BuildingComponent* building = myBuilding->GetComponent<BuildingComponent>();
	if (myUnitCount + building->GetUnitSupplyCost(eUnitType::GRUNT) > myUnitCap
		|| myGunpowder < building->GetUnitCost(eUnitType::GRUNT))
	{
		gruntValue = 0.f;
	}

	if (myHasUnlockedRanger == false || myUnitCount + building->GetUnitSupplyCost(eUnitType::RANGER) > myUnitCap
		|| myGunpowder < building->GetUnitCost(eUnitType::RANGER))
	{
		rangerValue = 0.f;
	}
	if (myHasUnlockedTank == false || myUnitCount + building->GetUnitSupplyCost(eUnitType::TANK) > myUnitCap
		|| myGunpowder < building->GetUnitCost(eUnitType::TANK))
	{
		tankValue = 0.f;
	}

	set.AddValue(static_cast<int>(eFuzzyAI::SPAWN_GRUNT), gruntValue);
	set.AddValue(static_cast<int>(eFuzzyAI::SPAWN_RANGER), rangerValue);
	set.AddValue(static_cast<int>(eFuzzyAI::SPAWN_TANK), tankValue);

	return set;
}

CU::FuzzySet AIDirector::UpdateRoamingAdvisor()
{
	CU::FuzzySet set(static_cast<int>(eFuzzyAI::_COUNT));

	set.AddValue(static_cast<int>(eFuzzyAI::ROAM), 0.1f * myIdleUnits.Size());

	return set;
}

bool AIDirector::UpdateCurrentAction()
{
	if (myDecisionTimer.ReachedMax())
	{
		myCurrentAction.myIsDone = true;
		return myCurrentAction.myIsDone;
	}

	switch (myCurrentAction.myFuzzyAction)
	{
	case eFuzzyAI::SPAWN_GRUNT:
		myAIAction = "SPAWN_GRUNT";
		myCurrentAction.myIsDone = myBuilding->GetComponent<BuildingComponent>()->GetCurrentBuildTime() <= 0.f;
		break;
	case eFuzzyAI::SPAWN_RANGER:
		myAIAction = "SPAWN_RANGER";
		myCurrentAction.myIsDone = myBuilding->GetComponent<BuildingComponent>()->GetCurrentBuildTime() <= 0.f;
		break;
	case eFuzzyAI::SPAWN_TANK:
		myAIAction = "SPAWN_TANK";
		myCurrentAction.myIsDone = myBuilding->GetComponent<BuildingComponent>()->GetCurrentBuildTime() <= 0.f;
		break;
	case eFuzzyAI::TAKE_RESOURCE_POINT:
		myAIAction = "TAKE_RESOURCE_POINT";
		UpdateTakeControlPoints(myCurrentAction.myFuzzyAction);
		break;
	case eFuzzyAI::TAKE_VICTORY_POINT:
		myAIAction = "TAKE_VICTORY_POINT";
		UpdateTakeControlPoints(myCurrentAction.myFuzzyAction);
		break;
	case eFuzzyAI::TAKE_ARTIFACT:
		myAIAction = "TAKE_ARTIFACT";
		myCurrentAction.myIsDone = myUpgradeData.myArtifactHunters.Size() == 0;
		break;
	case eFuzzyAI::UPGRADE_GRUNT:
		myAIAction = "UPGRADE_GRUNT";
		myUpgradeData.myGruntKillCount = 0;
		myCurrentAction.myIsDone = true;
		break;
	case eFuzzyAI::UPGRADE_RANGER:
		myAIAction = "UPGRADE_RANGER";
		myUpgradeData.myRangerKillCount = 0;
		myCurrentAction.myIsDone = true;
		break;
	case eFuzzyAI::UPGRADE_TANK:
		myAIAction = "UPGRADE_TANK";
		myUpgradeData.myTankKillCount = 0;
		myCurrentAction.myIsDone = true;
		break;
	case eFuzzyAI::ROAM:
		myAIAction = "ROAM";
		myCurrentAction.myIsDone = true;
		break;
	case eFuzzyAI::DO_NOTHING:
		myAIAction = "DO_NOTHING";
		myCurrentAction.myIsDone = true;
		break;
	default:
		break;
	}

	DEBUG_PRINT(myAIDecision);
	DEBUG_PRINT(myAIAction);


	if (myCurrentAction.myIsDone == true && myDecisionTimer.ReachedMin() == false)
	{
		myCurrentAction.myIsDone = false;
	}

	return myCurrentAction.myIsDone;
}

bool AIDirector::FuzzyDecisionDone()
{
	return myActionQueue.Size() == 0 && myCurrentAction.myIsDone == true;
}

void AIDirector::InterpretFuzzySet()
{
	DL_ASSERT_EXP(myActionQueue.Size() == 0, "AIDIRECTOR: Queuesize is more than zero when executing fuzzy action");
	eFuzzyAI action = static_cast<eFuzzyAI>(myFuzzySet->GetHighersMember());

	switch (action)
	{
	case eFuzzyAI::SPAWN_GRUNT:
		myAIDecision = "SPAWN_GRUNT";
		myActionQueue.InsertFirst(Action(action));
		break;
	case eFuzzyAI::SPAWN_RANGER:
		myAIDecision = "SPAWN_RANGER";
		myActionQueue.InsertFirst(Action(action));
		break;
	case eFuzzyAI::SPAWN_TANK:
		myAIDecision = "SPAWN_TANK";
		myActionQueue.InsertFirst(Action(action));
		break;
	case eFuzzyAI::TAKE_RESOURCE_POINT:
		myAIDecision = "TAKE_RESOURCE_POINT";
		HandleControlPoints(action);
		break;
	case eFuzzyAI::TAKE_VICTORY_POINT:
		myAIDecision = "TAKE_VICTORY_POINT";
		HandleControlPoints(action);
		break;
	case eFuzzyAI::TAKE_ARTIFACT:
		myAIDecision = "TAKE_ARTIFACT";
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::UPGRADE_GRUNT:
		myAIDecision = "UPGRADE_GRUNT";
		HandleUpgrades(action);
		break;
	case eFuzzyAI::UPGRADE_RANGER:
		myAIDecision = "UPGRADE_RANGER";
		HandleUpgrades(action);
		break;
	case eFuzzyAI::UPGRADE_TANK:
		myAIDecision = "UPGRADE_TANK";
		HandleUpgrades(action);
		break;
	case eFuzzyAI::ROAM:
		myAIDecision = "ROAM";
		HandleRoaming();
		break;
	default:
		break;
	}

	myCurrentAction.myFuzzyAction = eFuzzyAI::DO_NOTHING;
	myCurrentAction.myIsDone = true;
}

void AIDirector::StartNextAction()
{
	if (myActionQueue.Size() == 0)
	{
		myCurrentAction.myFuzzyAction = eFuzzyAI::DO_NOTHING;
		myCurrentAction.myIsDone = true;
		return;
	}

	myCurrentAction = myActionQueue.GetLast();
	myActionQueue.RemoveCyclicAtIndex(myActionQueue.Size() - 1);

	switch (myCurrentAction.myFuzzyAction)
	{
	case eFuzzyAI::SPAWN_GRUNT:
		Director::SpawnUnit(eUnitType::GRUNT);
		break;
	case eFuzzyAI::SPAWN_RANGER:
		Director::SpawnUnit(eUnitType::RANGER);
		break;
	case eFuzzyAI::SPAWN_TANK:
		Director::SpawnUnit(eUnitType::TANK);
		break;
	case eFuzzyAI::TAKE_RESOURCE_POINT:
		//Does not need to do anything here, everything is handled in UpdateCurrentAction
		break;
	case eFuzzyAI::TAKE_VICTORY_POINT:
		//Does not need to do anything here, everything is handled in UpdateCurrentAction
		break;
	case eFuzzyAI::TAKE_ARTIFACT:
	{
		if (myIdleUnits.Size() > 0)
		{
			Entity* unit = myIdleUnits[0];

			float closest = FLT_MAX;
			CU::Vector2<float> target = unit->GetPosition();

			const CU::GrowingArray<Entity*> artifacts = PollingStation::GetInstance()->GetArtifacts();
			for (int i = 0; i < artifacts.Size(); ++i)
			{
				float blockValue = myMaps.myBlockMap->GetValue(artifacts[i]->GetPosition());
				if (blockValue > 0)
				{
					continue;
				}

				float distance2 = CU::Length2(unit->GetPosition() - artifacts[i]->GetPosition());
				if (distance2 < closest)
				{
					closest = distance2;
					target = artifacts[i]->GetPosition();
				}
			}

			CU::Vector3<float> target3d;
			target3d.x = target.x;
			target3d.z = target.y;
			bool quiet = true;
			unit->GetComponent<ControllerComponent>()->AttackMove(target3d, true, quiet);

			myUpgradeData.myArtifactHunters.Add(unit);
		}

		break;
	}
	case eFuzzyAI::UPGRADE_GRUNT:
		Director::UpgradeUnit(eUnitType::GRUNT);
		break;
	case eFuzzyAI::UPGRADE_RANGER:
		Director::UpgradeUnit(eUnitType::RANGER);
		break;
	case eFuzzyAI::UPGRADE_TANK:
		Director::UpgradeUnit(eUnitType::TANK);
		break;
	case eFuzzyAI::ROAM:
	{
		int controllPoint = PollingStation::GetInstance()->GetVictoryAndResourcePoints().Size();
		controllPoint = rand() % controllPoint;
		Entity* point = PollingStation::GetInstance()->GetVictoryAndResourcePoints()[controllPoint];
		CU::Vector3<float> targetPos(point->GetPosition().x, 0.f, point->GetPosition().y);

		for (int i = 0; i < myIdleUnits.Size() && i < 3; ++i)
		{
			CU::Vector3<float> dir = myIdleUnits[i]->GetOrientation().GetPos() - targetPos;
			CU::Normalize(dir);

			bool quiet = true;
			myIdleUnits[i]->GetComponent<ControllerComponent>()->AttackMove(targetPos + (dir*2.f), true, quiet);
		}
		break;
	}
	default:
		break;
	}
}

void AIDirector::HandleControlPoints(eFuzzyAI aAction)
{
	int squadSize = 3;
	if (aAction == eFuzzyAI::TAKE_RESOURCE_POINT)
	{
		squadSize = myResourceData.myResourceSquadSize;
	}
	else if (aAction == eFuzzyAI::TAKE_VICTORY_POINT)
	{
		squadSize = myResourceData.myVictorySquadSize;
	}
	else
	{
		DL_ASSERT("Invalid eFuzzyAction in HandleControlPoints");
	}

	int remainingUnits = myIdleUnits.Size() - squadSize;

	if (remainingUnits > 0)
	{
		float maxDist = sqrt(256.f * 256.f + 256.f * 256.f);
		float distWeight = 10.f;
		float blockWeight = 23.f;
		float playerInfluenceWeight = 6.f;

		float bestResult = FLT_MAX;
		Entity* bestPoint = nullptr;



		const CU::GrowingArray<Entity*>* points = nullptr;

		if (aAction == eFuzzyAI::TAKE_RESOURCE_POINT)
		{
			points = &PollingStation::GetInstance()->GetResourcePoints();
		}
		else if (aAction == eFuzzyAI::TAKE_VICTORY_POINT)
		{
			points = &PollingStation::GetInstance()->GetVictoryPoints();
		}
		else
		{
			DL_ASSERT("Invalid eFuzzyAction in HandleControlPoints");
			points = &PollingStation::GetInstance()->GetVictoryPoints();
		}
		
		for (int i = 0; i < points->Size(); ++i)
		{
			Entity* point = (*points)[i];

			if (point->GetComponent<TriggerComponent>()->GetOwnerGainingPoint() ==
				myOwner)
			{
				continue;
			}

			CU::Vector2<float> pos = point->GetPosition();

			float dist = (CU::Length(pos - myBuilding->GetPosition()) + 0.000001f) / maxDist;
			float blockMapValue = myMaps.myBlockMap->GetValue(pos) * blockWeight;
			float playerInfluenceMapValue = myMaps.myPlayerInfluenceMap->GetValue(pos) * playerInfluenceWeight;

			float result = (blockMapValue)-(playerInfluenceMapValue);
			if (result <= 0.001f)
			{
				result = abs(result);
				result += dist * distWeight;

				if (result < bestResult)
				{
					bestResult = result;
					bestPoint = point;
				}
			}
		}

		if (bestPoint != nullptr)
		{
			myActionQueue.InsertFirst(Action(aAction, bestPoint->GetPosition()));
		}
		else
		{
			myActionQueue.InsertFirst(Action(eFuzzyAI::DO_NOTHING));
		}
	}
	else
	{
		float grunt = myFuzzySet->GetValue(static_cast<int>(eFuzzyAI::SPAWN_GRUNT));
		float ranger = myFuzzySet->GetValue(static_cast<int>(eFuzzyAI::SPAWN_RANGER));
		float tank = myFuzzySet->GetValue(static_cast<int>(eFuzzyAI::SPAWN_TANK));
		for (int i = 0; i < abs(remainingUnits); ++i)
		{
			if (myHasUnlockedTank == true && tank > ranger && tank > grunt)
			{
				myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_TANK));
			}
			else if (myHasUnlockedRanger == true && ranger > grunt)
			{
				myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_RANGER));
			}
			else
			{
				myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_GRUNT));
			}
		}

		CU::Vector2<float> target;

		if (aAction == eFuzzyAI::TAKE_RESOURCE_POINT)
		{
			target = PollingStation::GetInstance()->GetClosestNotOwnedResourcePoint(myOwner
				, myBuilding->GetPosition());
		}
		else if (aAction == eFuzzyAI::TAKE_VICTORY_POINT)
		{
			target = PollingStation::GetInstance()->GetClosestNotOwnedVictoryPoint(myOwner
				, myBuilding->GetPosition());
		}
		else
		{
			DL_ASSERT("Invalid eFuzzyAction in HandleControlPoints");
			target = myBuilding->GetPosition();
		}

		myActionQueue.InsertFirst(Action(aAction, target));
	}
}

void AIDirector::HandleUpgrades(eFuzzyAI aAction)
{
	BuildingComponent* building = myBuilding->GetComponent<BuildingComponent>();

	int cost = -1;
	switch (aAction)
	{
	case eFuzzyAI::UPGRADE_GRUNT:
		cost = building->GetUpgradeCost(eUnitType::GRUNT);
		break;
	case eFuzzyAI::UPGRADE_RANGER:
		cost = building->GetUpgradeCost(eUnitType::RANGER);
		break;
	case eFuzzyAI::UPGRADE_TANK:
		cost = building->GetUpgradeCost(eUnitType::TANK);
		break;
	default:
		DL_ASSERT("Invalid Action in AIDirector::HandleUpgrades");
		cost = building->GetUpgradeCost(eUnitType::GRUNT);
		break;
	}

	if (myArtifacts >= cost)
	{
		myActionQueue.InsertFirst(Action(aAction));
	}
	else
	{
		int artifactDiff = cost - myArtifacts;

		if (myIdleUnits.Size() < artifactDiff)
		{
			int neededUnits = artifactDiff - myIdleUnits.Size();
			for (int i = 0; i < neededUnits; ++i)
			{
				myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_GRUNT));
			}
		}

		for (int i = 0; i < artifactDiff; ++i)
		{
			myActionQueue.InsertFirst(Action(eFuzzyAI::TAKE_ARTIFACT));
		}
	}
}

void AIDirector::HandleRoaming()
{
	int remainingSupply = myUnitCap - myUnitCount;
	if (remainingSupply > 0)
	{
		CU::FuzzySet spawnSet = UpdateSpawningAdvisor();

		eUnitType unitToSpawn = eUnitType::NOT_A_UNIT;
		bool foundUnitToSpawn = false;
		while (foundUnitToSpawn == false)
		{
			int bestUnit = spawnSet.GetHighersMember();
			if (bestUnit == -1)
			{
				unitToSpawn = eUnitType::GRUNT;
				foundUnitToSpawn = true;
			}
			else
			{
				spawnSet.AddValue(bestUnit, -spawnSet.GetValue(bestUnit));

				unitToSpawn = static_cast<eUnitType>(bestUnit);
				if (remainingSupply >= myBuilding->GetComponent<BuildingComponent>()->GetUnitSupplyCost(unitToSpawn))
				{
					foundUnitToSpawn = true;
				}
			}
			
		}

		switch (unitToSpawn)
		{
		case NOT_A_UNIT:
			myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_GRUNT));
			break;
		case GRUNT:
			myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_GRUNT));
			break;
		case RANGER:
			myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_RANGER));
			break;
		case TANK:
			myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_TANK));
			break;
		default:
			myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_GRUNT));
			break;
		}
	}

	myActionQueue.InsertFirst(Action(eFuzzyAI::ROAM));
}

void AIDirector::UpdateTakeControlPoints(eFuzzyAI aAction)
{
	if (myActiveUnits.Size() == 0)
	{
		myCurrentAction.myIsDone = true;
		return;
	}

	myCurrentAction.myIsDone = true;
	float blockWeight = 5.f;
	float playerInfluenceWeight = 3.f;
	float blockMapValue = myMaps.myBlockMap->GetValue(myCurrentAction.myPosition) * blockWeight;
	float playerInfluenceMapValue = myMaps.myPlayerInfluenceMap->GetValue(myCurrentAction.myPosition) * playerInfluenceWeight;

	if (blockMapValue - playerInfluenceMapValue > 0.001f)
	{
		myCurrentAction.myIsDone = true;
		return;
	}

	int squadSize = 3;
	if (aAction == eFuzzyAI::TAKE_RESOURCE_POINT)
	{
		squadSize = myResourceData.myResourceSquadSize;
	}
	else if (aAction == eFuzzyAI::TAKE_VICTORY_POINT)
	{
		squadSize = myResourceData.myVictorySquadSize;
	}
	else
	{
		DL_ASSERT("Invalid eFuzzyAction in HandleControlPoints");
	}

	int readyCount = 0;
	for (int i = 0; i < myIdleUnits.Size(); ++i)
	{
		if (myIdleUnits[i]->GetComponent<ControllerComponent>()->IsReady() == true)
		{
			++readyCount;
		}
	}

	myCurrentAction.myIsDone = false;
	if (readyCount >= squadSize)
	{
		CU::Vector3<float> target;
		target.x = myCurrentAction.myPosition.x;
		target.z = myCurrentAction.myPosition.y;
		bool quiet = true;
		for (int i = 0; i < squadSize; ++i)
		{
			myIdleUnits[i]->GetComponent<ControllerComponent>()->AttackMove(target, true, quiet);
		}
		myCurrentAction.myIsDone = true;
	}
}