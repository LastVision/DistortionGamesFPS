#include "stdafx.h"
#include <AudioInterface.h>
#include "ButtonWidget.h"
#include <Engine.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include "../Game/PlayerDirector.h"

namespace GUI
{
	ButtonWidget::ButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer)
		: Widget()
		, myImageNormal(nullptr)
		, myImagePressed(nullptr)
		, myImageHover(nullptr)
		, myImageCurrent(nullptr)
		, myClickEvent(nullptr)
		, myHoverText("")
	{
		std::string spritePathNormal = "";
		std::string spritePathHover = "";
		std::string spritePathPressed = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritenormal"), "path", spritePathNormal);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritehover"), "path", spritePathHover);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritepressed"), "path", spritePathPressed);

		tinyxml2::XMLElement* hoverElement = aReader->FindFirstChild(anXMLElement, "hover");
		if (hoverElement != nullptr)
		{
			aReader->ForceReadAttribute(hoverElement, "text", myHoverText);
		}

		ReadTooltip(aReader, anXMLElement, aPlayer);
		ReadEvent(aReader, anXMLElement);

		myImageNormal = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathNormal, mySize, mySize / 2.f);
		myImageHover = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathHover, mySize, mySize / 2.f);
		myImagePressed = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathPressed, mySize, mySize / 2.f);
		myImageCurrent = myImageNormal;
	}

	ButtonWidget::~ButtonWidget()
	{
		SAFE_DELETE(myImageNormal);
		SAFE_DELETE(myImagePressed);
		SAFE_DELETE(myImageHover);
		SAFE_DELETE(myClickEvent);
		SAFE_DELETE(myTooltipInfo);
		myImageCurrent = nullptr;
	}

	void ButtonWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myImageCurrent->Render(myPosition + aParentPosition);

		if (myImageCurrent == myImageHover && myHoverText != "")
		{
			CU::Vector2<float> hoverPosition = { myPosition.x + mySize.x / 2.f, myPosition.y };
			hoverPosition += aParentPosition;
		
			Prism::Engine::GetInstance()->PrintText(myHoverText, hoverPosition, Prism::eTextType::RELEASE_TEXT);
		}
	}

	void ButtonWidget::OnMousePressed(const CU::Vector2<float>&)
	{
		myImageCurrent = myImagePressed;
	}

	void ButtonWidget::OnMouseUp()
	{
		Click();
		myImageCurrent = myImageNormal;
	}

	void ButtonWidget::OnMouseEnter()
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("buttonHover", 0);
		myImageCurrent = myImageHover;
	}

	void ButtonWidget::OnMouseExit()
	{
		myImageCurrent = myImageNormal;
	}

	void ButtonWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewSize, anOldSize, aIsFullScreen);
		myImageNormal->SetSize(mySize, mySize / 2.f);
		myImagePressed->SetSize(mySize, mySize / 2.f);
		myImageHover->SetSize(mySize, mySize / 2.f);
	}

	bool ButtonWidget::IsInside(const CU::Vector2<float>& aPosition) const
	{
		return aPosition.x >= myPosition.x - myImageCurrent->GetHotspot().x &&
			aPosition.x <= myPosition.x + mySize.x - myImageCurrent->GetHotspot().x &&
			aPosition.y >= myPosition.y - myImageCurrent->GetHotspot().y &&
			aPosition.y <= myPosition.y + mySize.y - myImageCurrent->GetHotspot().y;
	}

	void ButtonWidget::SetPosition(const CU::Vector2<float>& aPosition, bool aIsHotspot)
	{
		if (aIsHotspot == true)
		{
			myPosition = { aPosition.x + myImageCurrent->GetHotspot().x, aPosition.y - myImageCurrent->GetHotspot().y };
		}
		else
		{
			myPosition = aPosition;
		}
	}

	void ButtonWidget::ReadTooltip(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer)
	{
		if (aReader->FindFirstChild(anXMLElement, "tooltip") != nullptr)
		{
			std::string headline = "";
			std::string text = "";
			std::string action = "";
			int upgradeLevel = -1;
			bool isLargeTooltip = true;

			aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "tooltip"), "headline", headline);
			aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "tooltip"), "text", text);
			aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "tooltip"), "islargetooltip", isLargeTooltip);
			aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "tooltip"), "action", action);
			aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "tooltip"), "upgradelevel", upgradeLevel);

			myTooltipInfo = new TooltipInfo(headline, text, isLargeTooltip);

			if (action == "totem")
			{
				//myTooltipInfo->myCooldown = &aPlayer->GetTotemMaxCooldown();
				//myTooltipInfo->myCooldownType = eCooldownType::TOTEM;
			}
			else if (action == "spawn_grunt")
			{
				myTooltipInfo->myGunpowderCost = &aPlayer->GetUnitCost(0);
				myTooltipInfo->mySupplyCost = &aPlayer->GetUnitSupplyCost(0);
			}
			else if (action == "spawn_ranger")
			{
				myTooltipInfo->myGunpowderCost = &aPlayer->GetUnitCost(1);
				myTooltipInfo->mySupplyCost = &aPlayer->GetUnitSupplyCost(1);
			}
			else if (action == "spawn_tank")
			{
				myTooltipInfo->myGunpowderCost = &aPlayer->GetUnitCost(2);
				myTooltipInfo->mySupplyCost = &aPlayer->GetUnitSupplyCost(2);
			}
			else if (action == "upgrade_grunt")
			{
				DL_ASSERT_EXP(upgradeLevel != -1, "[ButtonWidget] action 'upgrade_grunt' needs an 'upgradelevel'");
				//myTooltipInfo->myCooldown = &aPlayer->GetUpgradeMaxCooldown(0);
				//myTooltipInfo->myCooldownType = eCooldownType::UPGRADE;
				myTooltipInfo->myUpgradeLevel = 0;
				myTooltipInfo->myArftifactCost = &aPlayer->GetUpgradeCost(0, upgradeLevel);
			}
			else if (action == "upgrade_ranger")
			{
				DL_ASSERT_EXP(upgradeLevel != -1, "[ButtonWidget] action 'upgrade_ranger' needs an 'upgradelevel'");
				//myTooltipInfo->myCooldown = &aPlayer->GetUpgradeMaxCooldown(1);
				//myTooltipInfo->myCooldownType = eCooldownType::UPGRADE;
				myTooltipInfo->myUpgradeLevel = 1;
				myTooltipInfo->myArftifactCost = &aPlayer->GetUpgradeCost(1, upgradeLevel);
			}
			else if (action == "upgrade_tank")
			{
				DL_ASSERT_EXP(upgradeLevel != -1, "[ButtonWidget] action 'upgrade_tank' needs an 'upgradelevel'");
				//myTooltipInfo->myCooldown = &aPlayer->GetUpgradeMaxCooldown(2);
				//myTooltipInfo->myCooldownType = eCooldownType::UPGRADE;
				myTooltipInfo->myUpgradeLevel = 2;
				myTooltipInfo->myArftifactCost = &aPlayer->GetUpgradeCost(2, upgradeLevel);
			}
		}
	}

	void ButtonWidget::ReadEvent(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
	{
		std::string clickEvent = "";
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "event", clickEvent);

		if (clickEvent == "none") // for passive ability buttons
		{
		}
		else if (clickEvent == "game_start")
		{
			int id = 0;
			aReader->ReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", id);
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_START, id);
		}
		else if (clickEvent == "game_start_easy")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_START_EASY);
		}
		else if (clickEvent == "game_start_normal")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_START_NORMAL);
		}
		else if (clickEvent == "game_start_hard")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_START_HARD);
		}
		else if (clickEvent == "game_level_select")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_LEVEL_SELECT);
		}
		else if (clickEvent == "game_restart")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_RESTART);
		}
		else if (clickEvent == "game_credit")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_CREDIT);
		}
		else if (clickEvent == "game_quit")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_QUIT);
		}
		else if (clickEvent == "game_help")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_HELP);
		}
		else if (clickEvent == "resume_game")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::RESUME_GAME);
		}
		else if (clickEvent == "action_move")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT_ACTION_MOVE);
		}
		else if (clickEvent == "action_attack")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT_ACTION_ATTACK);
		}
		else if (clickEvent == "action_stop")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT_ACTION_STOP);
		}
		else if (clickEvent == "action_place_totem")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::PLACE_TOTEM);
		}
		else if (clickEvent == "action_move_attack")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT_ACTION_ATTACK_MOVE);
		}
		else if (clickEvent == "action_stand_ground")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT_ACTION_STAND_GROUND);
		}
		else if (clickEvent == "action_enrage")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::ENRAGE);
		}
		else if (clickEvent == "spawn_unit")
		{
			int ID = -1;
			aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", ID);
			myClickEvent = new OnClickMessage(eOnClickEvent::SPAWN_UNIT, ID);
		}
		else if (clickEvent == "spawn_locked_unit")
		{
			int ID = -1;
			aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", ID);
			myClickEvent = new OnClickMessage(eOnClickEvent::SPAWN_LOCKED_UNIT, ID);
		}
		else if (clickEvent == "select_control_group")
		{
			int index = -1;
			aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", index);
			myClickEvent = new OnClickMessage(eOnClickEvent::SELECT_CONTROL_GROUP, index);
		}
		else if (clickEvent == "upgrade_unit")
		{
			int unitID = -1;
			aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", unitID);
			myClickEvent = new OnClickMessage(eOnClickEvent::UPGRADE_UNIT, unitID);
		}
		else if (clickEvent == "IncreaseVolume")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::INCREASEVOLUME);
		}
		else if (clickEvent == "LowerVolume")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::LOWERVOLUME);
		}
		else if (clickEvent == "IncreaseMusic")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::INCREASEMUSIC);
		}
		else if (clickEvent == "LowerMusic")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::LOWERMUSIC);
		}
		else if (clickEvent == "IncreaseVoice")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::INCREASEVOICE);
		}
		else if (clickEvent == "LowerVoice")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::LOWERVOICE);
		}
		else if (clickEvent == "options_menu")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS_MENU);
		}
		else if (clickEvent == "toggle_shadows")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::TOGGLE_SHADOWS);
		}
		else
		{
			std::string message = "[ButtonWidget]: No onclick event named " + clickEvent;
			DL_ASSERT(message);
		}
	}

	void ButtonWidget::Click()
	{
		if (myClickEvent != nullptr)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("buttonClick", 0);
			PostMaster::GetInstance()->SendMessage(*myClickEvent);
		}
	}
}