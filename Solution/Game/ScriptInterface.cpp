#include "stdafx.h"

#include <ArtifactMessage.h>
#include <CinematicMessage.h>
#include "Console.h"
#include <Entity.h>
#include <EntityId.h>
#include <GameStateMessage.h>
#include <KillUnitMessage.h>
#include <MoveCameraMessage.h>
#include <PostMaster.h>
#include <ResourceMessage.h>
#include <RunScriptMessage.h>
#include "ScriptInterface.h"
#include <ScriptSystem.h>
#include <SpawnUnitMessage.h>
#include <TextMessage.h>
#include <TimeMultiplierMessage.h>
#include <TriggerComponent.h>
#include <ToggleBuildTimeMessage.h>
#include <ToggleGUIMessage.h>
#include <ToggleFogOfWarMessage.h>
#include <ToggleRenderLinesMessage.h>
#include <VictoryMessage.h>

namespace Script_Interface
{
	int Print(lua_State* aState) //std::string aString
	{
		std::string stringArg = lua_tostring(aState, 1);

		DEBUG_PRINT_LUA(stringArg);

		return 0;
	}

	int PrintConsole(lua_State* aState) //std::string aString
	{
		std::string stringArg = lua_tostring(aState, 1);

		//Console::GetInstance()->AddHistory(stringArg, eHistoryType::WARNING);
		return 0;
	}

	int MoveCamera(lua_State* aState) //int aX, int aY, int aDistance
	{
		float x = float(lua_tonumber(aState, 1));
		float y = float(lua_tonumber(aState, 2));
		float distance = float(lua_tonumber(aState, 3));


		CU::Vector2<float> moveAmount = CU::Vector2<float>(x, y);
		CU::Normalize(moveAmount);
		moveAmount *= distance;

		PostMaster::GetInstance()->SendMessage(MoveCameraMessage(moveAmount, eHowToHandleMovement::ZERO_TO_ONE));

		return 0;
	}

	int RenderBox(lua_State* aState) //int aCenterX, int aCenterY, int aCenterZ, int aSize, int aColor, bool aWireframe
	{
		float x = float(lua_tonumber(aState, 1));
		float y = float(lua_tonumber(aState, 2));
		float z = float(lua_tonumber(aState, 3));

		float size = float(lua_tonumber(aState, 4));
		int color = int(lua_tonumber(aState, 5));
		bool wireframe = lua_toboolean(aState, 6) == 0 ? false : true;

		Prism::RenderBox({ x, y, z }, static_cast<eColorDebug>(color), size, wireframe);

		return 0;
	}

	int RenderLine(lua_State* aState) //int aStartX, int aStartY, int aStartZ, int aEndX, int aEndY, int aEndZ, int aColor
	{
		float x1 = float(lua_tonumber(aState, 1));
		float y1 = float(lua_tonumber(aState, 2));
		float z1 = float(lua_tonumber(aState, 3));

		float x2 = float(lua_tonumber(aState, 4));
		float y2 = float(lua_tonumber(aState, 5));
		float z2 = float(lua_tonumber(aState, 6));

		int color = int(lua_tonumber(aState, 7));

		Prism::RenderLine3D({ x1, y1, z1 }, { x2, y2, z2 }, static_cast<eColorDebug>(color));

		return 0;
	}

	int StartCinematic(lua_State* aState)//int aCinematicIndex
	{
		int cinematicIndex = int(lua_tonumber(aState, 1));
		PostMaster::GetInstance()->SendMessage(CinematicMessage(cinematicIndex, eCinematicAction::START));

		return 0;
	}

	int EndCinematic(lua_State*)//void
	{
		PostMaster::GetInstance()->SendMessage(CinematicMessage(0, eCinematicAction::END));
		return 0;
	}

	int ShowGUI(lua_State* aState)//void
	{
		float fadeTime = float(lua_tonumber(aState, 1));
		PostMaster::GetInstance()->SendMessage(ToggleGUIMessage(true, fadeTime));
		return 0;
	}

	int HideGUI(lua_State* aState)//void
	{
		float fadeTime = float(lua_tonumber(aState, 1));
		PostMaster::GetInstance()->SendMessage(ToggleGUIMessage(false, fadeTime));
		return 0;
	}

	int ScriptRun(lua_State* aState)//string aScriptFile
	{
		std::string filePath = lua_tostring(aState, 1);
		PostMaster::GetInstance()->SendMessage(RunScriptMessage(filePath));
		return 0;
	}

	int HideNavMesh(lua_State*)//void
	{
		PostMaster::GetInstance()->SendMessage(ToggleRenderLinesMessage(false));
		return 0;
	}

	int ShowNavMesh(lua_State*)//void
	{
		PostMaster::GetInstance()->SendMessage(ToggleRenderLinesMessage(true));
		return 0;
	}

	int GetOwner(lua_State* aState)//void
	{
		int id = int(lua_tonumber(aState, 1));
		Entity* entity = EntityId::GetInstance()->GetEntity(id);

		lua_pushinteger(aState, static_cast<int>(entity->GetOwner()));
		return 1;
	}

	int GetTrigger(lua_State* aState)//void
	{
		eEntityType type = static_cast<eEntityType>(int(lua_tonumber(aState, 1)));
		int id = int(lua_tonumber(aState, 2));
		Entity* entity = EntityId::GetInstance()->GetTrigger(type, id);
		if (entity == nullptr)
		{
			std::stringstream ss;
			ss << "Trigger " << id << " not found. Check ID.";

			DL_MESSAGE_BOX(ss.str().c_str(), "Trigger not Found!", MB_ICONWARNING);
			lua_pushinteger(aState, -1);
		}
		else
		{
			int entityId = entity->GetId();
			lua_pushinteger(aState, static_cast<int>(entityId));
		}
		return 1;
	}

	int ReloadLevel(lua_State*)//void
	{
		PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::RELOAD_LEVEL));
		return 0;
	}

	int SkipLevel(lua_State*)//void
	{
		PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::LOAD_NEXT_LEVEL));
		return 0;
	}

	int ModifyResource(lua_State* aState)//void
	{
		int directorId = int(lua_tonumber(aState, 1));
		int resourceModifier = int(lua_tonumber(aState, 2));

		eOwnerType owner = eOwnerType::NOT_USED;
		if (directorId == eOwnerType::PLAYER)
		{
			owner = eOwnerType::PLAYER;
		}
		else if (directorId == eOwnerType::ENEMY)
		{
			owner = eOwnerType::ENEMY;
		}

		if (owner == eOwnerType::NOT_USED || owner == eOwnerType::NEUTRAL)
		{
			return 0;
		}

		PostMaster::GetInstance()->SendMessage(ResourceMessage(owner, resourceModifier));
		return 0;
	}

	int ModifyVictory(lua_State* aState)//void
	{
		int directorId = int(lua_tonumber(aState, 1));
		int victoryModifier = int(lua_tonumber(aState, 2));

		eOwnerType owner = eOwnerType::NOT_USED;
		if (directorId == eOwnerType::PLAYER)
		{
			owner = eOwnerType::PLAYER;
		}
		else if (directorId == eOwnerType::ENEMY)
		{
			owner = eOwnerType::ENEMY;
		}

		if (owner == eOwnerType::NOT_USED || owner == eOwnerType::NEUTRAL)
		{
			return 0;
		}

		PostMaster::GetInstance()->SendMessage(VictoryMessage(owner, victoryModifier));
		return 0;
	}

	int ModifyOwnership(lua_State* aState)//void
	{
		int resourcePointID = int(lua_tonumber(aState, 1));
		int directorId = int(lua_tonumber(aState, 2));
		float ownershipModifier = float(lua_tonumber(aState, 3));

		eOwnerType owner = eOwnerType::NOT_USED;
		if (directorId == eOwnerType::PLAYER)
		{
			owner = eOwnerType::PLAYER;
		}
		else if (directorId == eOwnerType::ENEMY)
		{
			owner = eOwnerType::ENEMY;
		}
		else if (directorId == eOwnerType::NEUTRAL)
		{
			owner = eOwnerType::NEUTRAL;
		}

		if (owner == eOwnerType::NOT_USED)
		{
			return 0;
		}

		Entity* controlPoint = EntityId::GetInstance()->GetEntity(resourcePointID);

		if (controlPoint == nullptr || controlPoint->GetComponent<TriggerComponent>() == nullptr)
		{
			std::stringstream ss;
			ss << "Entity " << resourcePointID << " not found. Check ID.";

			DL_MESSAGE_BOX(ss.str().c_str(), "Entity not Found!", MB_ICONWARNING);
			lua_pushinteger(aState, -1);
		}
		else
		{
			owner = controlPoint->GetComponent<TriggerComponent>()->ModifyOwnership(owner, ownershipModifier);
		}

		lua_pushinteger(aState, static_cast<int>(owner));
		return 1;
	}

	int DisableAI(lua_State*)//void
	{
		PostMaster::GetInstance()->SendMessage(TimeMultiplierMessage(eOwnerType::ENEMY, 0.f));

		return 0;
	}

	int EnableAI(lua_State*)//void
	{
		PostMaster::GetInstance()->SendMessage(TimeMultiplierMessage(eOwnerType::ENEMY, 1.f));

		return 0;
	}

	int TimeMultiplier(lua_State* aState)//void
	{
		float speed = float(lua_tonumber(aState, 1));
		PostMaster::GetInstance()->SendMessage(TimeMultiplierMessage(eOwnerType::ENEMY, speed));
		PostMaster::GetInstance()->SendMessage(TimeMultiplierMessage(eOwnerType::PLAYER, speed));
		return 0;
	}

	int GiveGunpowder(lua_State*)
	{
		PostMaster::GetInstance()->SendMessage(ResourceMessage(eOwnerType::PLAYER, 1000));

		return 0;
	}

	int GiveArtifacts(lua_State*)
	{
		PostMaster::GetInstance()->SendMessage(ArtifactMessage(eOwnerType::PLAYER, 1000));

		return 0;
	}

	int GiveVictoryPoints(lua_State*)
	{
		PostMaster::GetInstance()->SendMessage(VictoryMessage(eOwnerType::PLAYER, 100));

		return 0;
	}

	int DisableBuildTime(lua_State*)
	{
		PostMaster::GetInstance()->SendMessage(ToggleBuildTimeMessage(true));

		return 0;
	}

	int EnableBuildTime(lua_State*)
	{
		PostMaster::GetInstance()->SendMessage(ToggleBuildTimeMessage(false));

		return 0;
	}

	int SpawnNeutral(lua_State* aState)
	{
		int unitType = int(lua_tonumber(aState, 1));
		float x = float(lua_tonumber(aState, 2));
		float z = float(lua_tonumber(aState, 3));

		CU::Vector2<float> pos(x, z);

		PostMaster::GetInstance()->SendMessage(SpawnUnitMessage(unitType, eOwnerType::NEUTRAL, pos, pos));

		return 0;
	}

	int UnlockUnits(lua_State*) // void
	{
		//Unlock units
		PostMaster::GetInstance()->SendMessage(KillUnitMessage(static_cast<int>(eUnitType::RANGER),
			static_cast<int>(eOwnerType::NEUTRAL), { 0.f, 0.f }, static_cast<int>(eOwnerType::PLAYER)));
		PostMaster::GetInstance()->SendMessage(KillUnitMessage(static_cast<int>(eUnitType::TANK), 
			static_cast<int>(eOwnerType::NEUTRAL), { 0.f, 0.f }, static_cast<int>(eOwnerType::PLAYER)));

		return 0;
	}

	int ToggleFogOfWar(lua_State*) // void
	{
		PostMaster::GetInstance()->SendMessage(ToggleFogOfWarMessage());
		return 0;
	}

}

void ScriptInterface::RegisterFunctions()
{
	LUA::ScriptSystem* system = LUA::ScriptSystem::GetInstance();
	system->RegisterFunction("Print", Script_Interface::Print, "aString", "Prints stuff to the screen");
	system->RegisterFunction("PrintConsole", Script_Interface::PrintConsole, "aString", "Prints a single message to the console.");
	system->RegisterFunction("MoveCamera", Script_Interface::MoveCamera, "aXDir, aZDir, aDistance"
		, "Moves the Camera InGame\n\taXDir and aZDir will get normalized in the function\n\taDistance needs to be muliplied with DeltaTime on the LUA-Side");
	system->RegisterFunction("RenderBox", Script_Interface::RenderBox, "aX, aY, aZ, aSize, aColor, aWireframe"
		, "Draws a Debugbox centered on (aX, aY, aZ) with HalfWidth(aSize).\n\tAvailable colors are:\n\t0: White\n\t1: Black\n\t2: Red\n\t3: Green\n\t4: Blue\n\t5: Pink\n\t6: Yellow");
	system->RegisterFunction("RenderLine", Script_Interface::RenderLine, "aStartX, aStartY, aStartZ, aEndX, aEndY, aEndZ, aColor"
		, "Draws a DebugLine:\n\t\t from: (startX, startY, startZ)\n\t\t to: (endX, endY, endZ).\n\tAvailable colors are:\n\t0: White\n\t1: Black\n\t2: Red\n\t3: Green\n\t4: Blue\n\t5: Pink\n\t6: Yellow");
	system->RegisterFunction("StartCinematic", Script_Interface::StartCinematic, "aCinematicIndex", "Starts a Cinematic with the given Index");
	system->RegisterFunction("EndCinematic", Script_Interface::EndCinematic, "void", "Stops any running Cinematic and returns to normal game-mode");
	system->RegisterFunction("ShowGUI", Script_Interface::ShowGUI, "aFadeTime", "Shows the GUI with a fade-in");
	system->RegisterFunction("HideGUI", Script_Interface::HideGUI, "aFadeTime", "Hides the GUI with a fade-out");
	system->RegisterFunction("ScriptRun", Script_Interface::ScriptRun, "aScriptFile", "Run the script file.");
	system->RegisterFunction("ShowNavMesh", Script_Interface::ShowNavMesh, "", "Shows the lines for the navigation mesh.");
	system->RegisterFunction("HideNavMesh", Script_Interface::HideNavMesh, "", "Hides the lines for the navigation mesh.");
	system->RegisterFunction("GetOwner", Script_Interface::GetOwner, "aUnit", "Returns ownerID of unit, ex: owner = GetOwner(aUnit)");
	system->RegisterFunction("GetTrigger", Script_Interface::GetTrigger, "aTrigger", "Returns the Entity id for trigger of the supplied input, ex: trigger0 = GetTrigger(0)");
	system->RegisterFunction("ReloadLevel", Script_Interface::ReloadLevel, "", "Reloads the current level.");
	system->RegisterFunction("ModifyResource", Script_Interface::ModifyResource, "aOwnerEnum, aResourceModifier", "Modifies resource of owner, ex: ModifyResource(eOwnerType.PLAYER, resourceGain)");
	system->RegisterFunction("ModifyVictory", Script_Interface::ModifyVictory, "aOwnerEnum, aVictoryModifier", "Modifies victory points of owner, ex: ModifyVictory(eOwnerType.PLAYER, victoryPointGain)");
	system->RegisterFunction("ModifyOwnership", Script_Interface::ModifyOwnership, "aTriggerId, aOwnerEnum, aResourceModifier", "Modifies ownership of trigger, ex:\n\t myResourcePoint0.myOwner = ModifyOwnership(myResourcePoint0.myId, eOwnerType.PLAYER, ownershipGain * aDelta)");
	system->RegisterFunction("DisableAI", Script_Interface::DisableAI, "", "Disables AI");
	system->RegisterFunction("EnableAI", Script_Interface::EnableAI, "", "Enables AI");
	system->RegisterFunction("TimeMultiplier", Script_Interface::TimeMultiplier, "aMultiplier", "Modifies the game time. ex: TimeMultiplier(0.1) //this is slow  ");
	system->RegisterFunction("SkipLevel", Script_Interface::SkipLevel, "", "You skip the current level, and goes to the next in the list. \nIf you are on the last level it will reload that level.");
	system->RegisterFunction("Gold", Script_Interface::GiveGunpowder, "", "You get 1000 Gunpowder.");
	system->RegisterFunction("Artifact", Script_Interface::GiveArtifacts, "", "You get 1000 Artifacts.");
	system->RegisterFunction("Victory", Script_Interface::GiveVictoryPoints, "", "You get 100 victory points.");
	system->RegisterFunction("NoBuild", Script_Interface::DisableBuildTime, "", "Removes the buildtime on units, instaspawn, only for player.");
	system->RegisterFunction("Build", Script_Interface::EnableBuildTime, "", "Enables the buildtime on units, not instaspawn, only for player.");
	system->RegisterFunction("SpawnNeutral", Script_Interface::SpawnNeutral, "aType, aX, aZ", "Spawns a Netural creen of type aType at position aX, aZ.");
	system->RegisterFunction("UnlockUnits", Script_Interface::UnlockUnits, "", "Unlocks all units so you can build them.");
	system->RegisterFunction("Fog", Script_Interface::ToggleFogOfWar, "", "Toggles the Fog Of War.");
}
