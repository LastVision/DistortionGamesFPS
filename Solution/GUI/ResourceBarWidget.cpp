#include "stdafx.h"
#include <Engine.h>
#include "ResourceBarWidget.h"
#include "../Game/PlayerDirector.h"
#include "../Game/AIDirector.h"
#include <SpriteAnimator.h>
#include <AudioInterface.h>

namespace GUI
{
	ResourceBarWidget::ResourceBarWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer, const AIDirector* anAI)
		: Widget()
		, myValueSprite(nullptr)
		, myTextScale(1.f)
		, myGainAnimation(nullptr)
		, myLastValue(0)
	{
		std::string valueSpritePath = "";
		std::string value = "";
		CU::Vector2<float> valueSpriteSize;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "value"), "type", value);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "value"), "positionx", myTextPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "value"), "positiony", myTextPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textscale"), "value", myTextScale);

		tinyxml2::XMLElement* spriteElement = aReader->FindFirstChild(anXMLElement, "valuesprite");
		if (spriteElement != nullptr)
		{
			aReader->ForceReadAttribute(spriteElement, "path", valueSpritePath);
			aReader->ForceReadAttribute(spriteElement, "sizex", valueSpriteSize.x);
			aReader->ForceReadAttribute(spriteElement, "sizey", valueSpriteSize.y);
			aReader->ForceReadAttribute(spriteElement, "positionx", mySpritePosition.x);
			aReader->ForceReadAttribute(spriteElement, "positiony", mySpritePosition.y);

			myValueSprite = Prism::ModelLoader::GetInstance()->LoadSprite(valueSpritePath, valueSpriteSize);
		}

		if (value == "player_gold")
		{
			myValue = &aPlayer->GetGunpowder();
		}
		else if (value == "player_victory_point")
		{
			myValue = &aPlayer->GetVictoryPoints();
			myLastValue = *myValue;
			myGainAnimation = new Prism::SpriteAnimator("Data/Resource/SpriteAnimation/VictoryPointGainAnimation.xml");
		}
		else if (value == "enemy_victory_point")
		{
			myValue = &anAI->GetVictoryPoints();
			myLastValue = *myValue;
			myGainAnimation = new Prism::SpriteAnimator("Data/Resource/SpriteAnimation/VictoryPointGainAnimation.xml");
		}
		else if (value == "player_artifact")
		{
			myValue = &aPlayer->GetArtifact();
		}
		else
		{
			assert("Wrong type in resourcebar XML");
		}
	}

	ResourceBarWidget::~ResourceBarWidget()
	{
		SAFE_DELETE(myGainAnimation);
		SAFE_DELETE(myValueSprite);
	}

	void ResourceBarWidget::Update(float aDelta)
	{
		if (myGainAnimation != nullptr)
		{
			if (myLastValue != *myValue && *myValue % 100 == 0)
			{
				myGainAnimation->RestartAnimation();
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("victoryGain", 0);
			}
			myLastValue = *myValue;
			myGainAnimation->Update(aDelta);
		}
	}

	void ResourceBarWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myValueSprite != nullptr)
		{
			myValueSprite->Render(myPosition + aParentPosition + mySpritePosition);
		}

		if (myGainAnimation != nullptr)
		{
			myGainAnimation->Render(myPosition + aParentPosition + myTextPosition);
		}

		Prism::Engine::GetInstance()->PrintText(*myValue, myPosition + aParentPosition + myTextPosition, Prism::eTextType::RELEASE_TEXT);
	}

	void ResourceBarWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);

		if (myIsFullscreen == false)
		{
			CU::Vector2<float> ratioTextPosition = myTextPosition / anOldWindowSize.x;
			myTextPosition = ratioTextPosition * aNewWindowSize.x;

			CU::Vector2<float> ratioSpritePosition = mySpritePosition / anOldWindowSize.x;
			mySpritePosition = ratioSpritePosition * aNewWindowSize.x;
		}
		else
		{
			CU::Vector2<float> ratioTextPosition = myTextPosition / anOldWindowSize;
			myTextPosition = ratioTextPosition * aNewWindowSize;

			CU::Vector2<float> ratioSpritePosition = mySpritePosition / anOldWindowSize;
			mySpritePosition = ratioSpritePosition * aNewWindowSize;
		}

		myTextScale = (myTextScale / anOldWindowSize.x) * aNewWindowSize.x; 
	}
}