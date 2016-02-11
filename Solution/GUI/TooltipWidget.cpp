#include "stdafx.h"
#include <Engine.h>
#include "TooltipInfo.h"
#include "TooltipWidget.h"
#include "GUIManager.h"
#include "../Game/PlayerDirector.h"

namespace GUI
{
	TooltipWidget::TooltipWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement
		, const GUIManager* aGuiManager, const PlayerDirector* aPlayer)
		: Widget()
		, myGuiManager(aGuiManager)
		, myCooldownSprite(nullptr)
		, myGunpowderSprite(nullptr)
		, mySupplySprite(nullptr)
		, myPlayer(aPlayer)
		, myActiveWidget(nullptr)
	{
		mySize = { 0.f, 0.f };
		myPosition = { 0.f, 0.f };

		myLargeTooltip = new TooltipBox;
		mySmallTooltip = new TooltipBox;

		std::string largeBackgroundPath = "";
		std::string smallBackgroundPath = "";
		std::string cooldownPath = "";
		std::string gunpowderPath = "";
		std::string supplyPath = "";
		std::string artifactPath = "";
		CU::Vector2<float> costSpriteSize;
		CU::Vector2<float> largeBackgroundSize;
		CU::Vector2<float> smallBackgroundSize;

		tinyxml2::XMLElement* largeTooltipElement = aReader->FindFirstChild(anXMLElement, "largetooltip");
		tinyxml2::XMLElement* smallTooltipElement = aReader->FindFirstChild(anXMLElement, "smalltooltip");

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "backgroundsize"), "x", largeBackgroundSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "backgroundsize"), "y", largeBackgroundSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "headlineposition"), "x", myLargeTooltip->myHeadlinePosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "headlineposition"), "y", myLargeTooltip->myHeadlinePosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "textposition"), "x", myLargeTooltip->myTextPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "textposition"), "y", myLargeTooltip->myTextPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "backgroundsprite"), "path", largeBackgroundPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "costspritesize"), "x", costSpriteSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "costspritesize"), "y", costSpriteSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "costtextoffset"), "x", myCostTextOffset.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "costtextoffset"), "y", myCostTextOffset.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "cooldownsprite"), "path", cooldownPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "gunpowdersprite"), "path", gunpowderPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "supplysprite"), "path", supplyPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "artifactsprite"), "path", artifactPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "headlinescale"), "value", myLargeTooltip->myHeadlineScale);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "textscale"), "value", myLargeTooltip->myTextScale);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "cooldownsprite"), "positionx", myCooldownPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "cooldownsprite"), "positiony", myCooldownPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "gunpowdersprite"), "positionx", myGunpowderPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "gunpowdersprite"), "positiony", myGunpowderPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "supplysprite"), "positionx", mySupplyPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "supplysprite"), "positiony", mySupplyPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "artifactsprite"), "positionx", myArtifactPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(largeTooltipElement, "artifactsprite"), "positiony", myArtifactPosition.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(smallTooltipElement, "backgroundsize"), "x", smallBackgroundSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(smallTooltipElement, "backgroundsize"), "y", smallBackgroundSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(smallTooltipElement, "headlineposition"), "x", mySmallTooltip->myHeadlinePosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(smallTooltipElement, "headlineposition"), "y", mySmallTooltip->myHeadlinePosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(smallTooltipElement, "textposition"), "x", mySmallTooltip->myTextPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(smallTooltipElement, "textposition"), "y", mySmallTooltip->myTextPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(smallTooltipElement, "backgroundsprite"), "path", smallBackgroundPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(smallTooltipElement, "headlinescale"), "value", mySmallTooltip->myHeadlineScale);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(smallTooltipElement, "textscale"), "value", mySmallTooltip->myTextScale);

		myLargeTooltip->myBackground = Prism::ModelLoader::GetInstance()->LoadSprite(largeBackgroundPath, largeBackgroundSize);
		mySmallTooltip->myBackground = Prism::ModelLoader::GetInstance()->LoadSprite(smallBackgroundPath, smallBackgroundSize);
		myCooldownSprite = Prism::ModelLoader::GetInstance()->LoadSprite(cooldownPath, costSpriteSize);
		myGunpowderSprite = Prism::ModelLoader::GetInstance()->LoadSprite(gunpowderPath, costSpriteSize);
		mySupplySprite = Prism::ModelLoader::GetInstance()->LoadSprite(supplyPath, costSpriteSize);
		myArtifactSprite = Prism::ModelLoader::GetInstance()->LoadSprite(artifactPath, costSpriteSize);
	}

	TooltipWidget::~TooltipWidget()
	{
		SAFE_DELETE(myLargeTooltip->myBackground);
		SAFE_DELETE(mySmallTooltip->myBackground);
		SAFE_DELETE(myLargeTooltip);
		SAFE_DELETE(mySmallTooltip);
		SAFE_DELETE(myCooldownSprite);
		SAFE_DELETE(myGunpowderSprite);
		SAFE_DELETE(mySupplySprite);
		SAFE_DELETE(myArtifactSprite);
	}

	void TooltipWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myActiveWidget = myGuiManager->GetActiveWidget();

		if (myActiveWidget != nullptr && myActiveWidget->GetTooltipInfo() != nullptr)
		{
			const TooltipInfo* tooltipInfo = myActiveWidget->GetTooltipInfo();
			CU::Vector2<float> position = myPosition + aParentPosition;

			if (tooltipInfo->myIsLargeTooltip == true)
			{
				myLargeTooltip->myBackground->Render(position);

				Prism::Engine::GetInstance()->PrintText(tooltipInfo->myHeadline, myLargeTooltip->myHeadlinePosition + position
					, Prism::eTextType::RELEASE_TEXT, myLargeTooltip->myHeadlineScale);

				Prism::Engine::GetInstance()->PrintText(tooltipInfo->myText, myLargeTooltip->myTextPosition + position
					, Prism::eTextType::RELEASE_TEXT, myLargeTooltip->myTextScale);

				RenderCost(position, tooltipInfo);
			}
			else
			{
				mySmallTooltip->myBackground->Render(position);

				Prism::Engine::GetInstance()->PrintText(tooltipInfo->myHeadline, mySmallTooltip->myHeadlinePosition + position
					, Prism::eTextType::RELEASE_TEXT, mySmallTooltip->myHeadlineScale);

				Prism::Engine::GetInstance()->PrintText(tooltipInfo->myText, mySmallTooltip->myTextPosition + position
					, Prism::eTextType::RELEASE_TEXT, mySmallTooltip->myTextScale);
			}
		}
	}

	void TooltipWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);

		if (myIsFullscreen == false)
		{
			myLargeTooltip->myHeadlinePosition = (myLargeTooltip->myHeadlinePosition / anOldWindowSize.x) * aNewWindowSize.x;
			myLargeTooltip->myTextPosition = (myLargeTooltip->myTextPosition / anOldWindowSize.x) * aNewWindowSize.x;
			mySmallTooltip->myHeadlinePosition = (mySmallTooltip->myHeadlinePosition / anOldWindowSize.x) * aNewWindowSize.x;
			mySmallTooltip->myTextPosition = (mySmallTooltip->myTextPosition / anOldWindowSize.x) * aNewWindowSize.x;
			myCostTextOffset = (myCostTextOffset / anOldWindowSize.x) * aNewWindowSize.x;
			myCooldownPosition = (myCooldownPosition / anOldWindowSize.x) * aNewWindowSize.x;
			myGunpowderPosition = (myGunpowderPosition / anOldWindowSize.x) * aNewWindowSize.x;
			mySupplyPosition = (mySupplyPosition / anOldWindowSize.x) * aNewWindowSize.x;
			myArtifactPosition = (myArtifactPosition / anOldWindowSize.x) * aNewWindowSize.x;
			myLargeTooltip->myBackground->SetSize((myLargeTooltip->myBackground->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, { 0.f, 0.f });
			mySmallTooltip->myBackground->SetSize((mySmallTooltip->myBackground->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, { 0.f, 0.f });

			CU::Vector2<float> costSize = (myCooldownSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x;
			myCooldownSprite->SetSize(costSize, { 0.f, 0.f });
			myGunpowderSprite->SetSize(costSize, { 0.f, 0.f });
			mySupplySprite->SetSize(costSize, { 0.f, 0.f });
			myArtifactSprite->SetSize(costSize, { 0.f, 0.f });
		}
		else
		{
			myLargeTooltip->myHeadlinePosition = (myLargeTooltip->myHeadlinePosition / anOldWindowSize) * aNewWindowSize;
			myLargeTooltip->myTextPosition = (myLargeTooltip->myTextPosition / anOldWindowSize) * aNewWindowSize;
			mySmallTooltip->myHeadlinePosition = (mySmallTooltip->myHeadlinePosition / anOldWindowSize) * aNewWindowSize;
			mySmallTooltip->myTextPosition = (mySmallTooltip->myTextPosition / anOldWindowSize) * aNewWindowSize;
			myCostTextOffset = (myCostTextOffset / anOldWindowSize) * aNewWindowSize;
			myCooldownPosition = (myCooldownPosition / anOldWindowSize) * aNewWindowSize;
			myGunpowderPosition = (myGunpowderPosition / anOldWindowSize) * aNewWindowSize;
			mySupplyPosition = (mySupplyPosition / anOldWindowSize) * aNewWindowSize;
			myArtifactPosition = (myArtifactPosition / anOldWindowSize) * aNewWindowSize;
			myLargeTooltip->myBackground->SetSize((myLargeTooltip->myBackground->GetSize() / anOldWindowSize) * aNewWindowSize, { 0.f, 0.f });
			mySmallTooltip->myBackground->SetSize((mySmallTooltip->myBackground->GetSize() / anOldWindowSize) * aNewWindowSize, { 0.f, 0.f });

			CU::Vector2<float> costSize = (myCooldownSprite->GetSize() / anOldWindowSize) * aNewWindowSize;
			myCooldownSprite->SetSize(costSize, { 0.f, 0.f });
			myGunpowderSprite->SetSize(costSize, { 0.f, 0.f });
			mySupplySprite->SetSize(costSize, { 0.f, 0.f });
			myArtifactSprite->SetSize(costSize, { 0.f, 0.f });
		}

		myLargeTooltip->myTextScale = (myLargeTooltip->myTextScale / anOldWindowSize.x) * aNewWindowSize.x;
		myLargeTooltip->myHeadlineScale = (myLargeTooltip->myHeadlineScale / anOldWindowSize.x) * aNewWindowSize.x;
		mySmallTooltip->myTextScale = (mySmallTooltip->myTextScale / anOldWindowSize.x) * aNewWindowSize.x;
		mySmallTooltip->myHeadlineScale = (mySmallTooltip->myHeadlineScale / anOldWindowSize.x) * aNewWindowSize.x;
	}

	void TooltipWidget::RenderCost(const CU::Vector2<float>& aParentPosition, const TooltipInfo* aTooltipInfo)
	{
		if (aTooltipInfo->myCooldown != nullptr)
		{
			CU::Vector4<float> color(1.f, 1.f, 1.f, 1.f);

			switch (aTooltipInfo->myCooldownType)
			{
			case eCooldownType::TOTEM:
				if (myPlayer->CanCastTotem() == false)
				{
					color = { 1.f, 0.f, 0.f, 1.f };
				}
				break;
			case eCooldownType::UPGRADE:
				if (myPlayer->CanUpgrade(aTooltipInfo->myUpgradeLevel) == false)
				{
					color = { 1.f, 0.f, 0.f, 1.f };
				}
				break;
			}

			myCooldownSprite->Render(aParentPosition + myCooldownPosition, { 1.f, 1.f }, color);
			Prism::Engine::GetInstance()->PrintText(*aTooltipInfo->myCooldown, aParentPosition + myCooldownPosition + myCostTextOffset
				, Prism::eTextType::RELEASE_TEXT, myLargeTooltip->myTextScale, color);
		}

		if (aTooltipInfo->myGunpowderCost != nullptr)
		{
			CU::Vector4<float> color(1.f, 1.f, 1.f, 1.f);
			if (myPlayer->CanAffordGunpowder(*aTooltipInfo->myGunpowderCost) == false)
			{
				color = { 1.f, 0.f, 0.f, 1.f };
			}

			myGunpowderSprite->Render(aParentPosition + myGunpowderPosition, { 1.f, 1.f }, color);
			Prism::Engine::GetInstance()->PrintText(*aTooltipInfo->myGunpowderCost, aParentPosition + myGunpowderPosition + myCostTextOffset
				, Prism::eTextType::RELEASE_TEXT, myLargeTooltip->myTextScale, color);
		}

		if (aTooltipInfo->mySupplyCost != nullptr)
		{
			CU::Vector4<float> color(1.f, 1.f, 1.f, 1.f);
			if (myPlayer->CanAffordSupply(*aTooltipInfo->mySupplyCost) == false)
			{
				color = { 1.f, 0.f, 0.f, 1.f };
			}

			mySupplySprite->Render(aParentPosition + mySupplyPosition, { 1.f, 1.f }, color);
			Prism::Engine::GetInstance()->PrintText(*aTooltipInfo->mySupplyCost, aParentPosition + mySupplyPosition + myCostTextOffset
				, Prism::eTextType::RELEASE_TEXT, myLargeTooltip->myTextScale, color);
		}

		if (aTooltipInfo->myArftifactCost != nullptr)
		{
			CU::Vector4<float> color(1.f, 1.f, 1.f, 1.f);
			if (myPlayer->CanAffordArtifact(*aTooltipInfo->myArftifactCost) == false)
			{
				color = { 1.f, 0.f, 0.f, 1.f };
			}

			myArtifactSprite->Render(aParentPosition + myArtifactPosition, { 1.f, 1.f }, color);
			Prism::Engine::GetInstance()->PrintText(*aTooltipInfo->myArftifactCost, aParentPosition + myArtifactPosition + myCostTextOffset
				, Prism::eTextType::RELEASE_TEXT, myLargeTooltip->myTextScale, color);
		}
	}
}
