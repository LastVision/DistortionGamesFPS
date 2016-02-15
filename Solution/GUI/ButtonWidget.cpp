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