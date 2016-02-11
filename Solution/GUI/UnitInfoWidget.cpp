#include "stdafx.h"
#include "../Game/GameDefines.h"
#include "BarWidget.h"
#include "ButtonWidget.h"
#include <Engine.h>
#include "../Entity/EnrageComponent.h"
#include "../Entity/BuildingComponent.h"
#include "../Entity/Entity.h"
#include "../Game/PlayerDirector.h"
#include "../Game/AIDirector.h"
#include "../Entity/ActorComponent.h"
#include "../Entity/ControllerComponent.h"
#include "../Entity/HealthComponent.h"
#include "../Entity/PromotionComponent.h"
#include <PostMaster.h>
#include <OnClickMessage.h>
#include <SelectUnitMessage.h>
#include "UnitInfoWidget.h"
#include "WidgetContainer.h"

namespace GUI
{
	UnitInfoWidget::UnitInfoWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement
		, const PlayerDirector* aPlayer	, const AIDirector* anAI)
		: Widget()
		, myUnits(aPlayer->GetSelectedUnits())
		, mySelectedType(eEntityType::_COUNT)
		, myPlayerBuilding(aPlayer->GetBuildingComponent())
		, myEnemyBuilding(anAI->GetBuildingComponent())
		, myBuildingTimer(nullptr)
		, myTextScale(1.f)
		, myEnemyColor(0.35f, 0.35f, 0.35f, 1.f)
		, myQueueButtons(aPlayer->GetBuildingComponent().GetMaxQueue())
		, myIsUnitSelected(false)
	{
		std::string gruntUnitPath = "";
		std::string rangerUnitPath = "";
		std::string tankUnitPath = "";
		std::string gruntPortraitPath = "";
		std::string rangerPortraitPath = "";
		std::string tankPortraitPath = "";
		std::string buildingPortraitPath = "";
		std::string statsSpritePath = "";
		std::string activeQueueOverlaySpritePath = "";
		std::string starSpritePath = "";
		CU::Vector2<float> unitSize;
		CU::Vector2<float> portraitSize;
		CU::Vector2<float> statsSize;
		CU::Vector2<float> abortPosition;
		CU::Vector2<float> starSize;
		int abortButtonsPerRow = 0;

		tinyxml2::XMLElement* abortButtonElement = aReader->FindFirstChild(anXMLElement, "widget");

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "x", unitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "y", unitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitsize"), "x", portraitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitsize"), "y", portraitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "statssize"), "x", statsSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "statssize"), "y", statsSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitposition"), "x", myUnitPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitposition"), "y", myUnitPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitposition"), "x", myPortraitPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitposition"), "y", myPortraitPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gruntunit"), "path", gruntUnitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "rangerunit"), "path", rangerUnitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "tankunit"), "path", tankUnitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gruntportrait"), "path", gruntPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "rangerportrait"), "path", rangerPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "tankportrait"), "path", tankPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buildingportrait"), "path", buildingPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "statssprite"), "path", statsSpritePath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "abortbuttonposition"), "x", abortPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "abortbuttonposition"), "y", abortPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "timerposition"), "x", myTimerPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "timerposition"), "y", myTimerPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "abortbuttonsperrow"), "value", abortButtonsPerRow);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "activequeueoverlay"), "path", activeQueueOverlaySpritePath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "starsprite"), "path", starSpritePath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "starsprite"), "positionx", myUpgradeStarPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "starsprite"), "positiony", myUpgradeStarPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "starsprite"), "sizex", starSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "starsprite"), "sizey", starSize.y);

		myPosition = { 0.f, 0.f };
		myGruntUnit = Prism::ModelLoader::GetInstance()->LoadSprite(gruntUnitPath, unitSize);
		myRangerUnit = Prism::ModelLoader::GetInstance()->LoadSprite(rangerUnitPath, unitSize);
		myTankUnit = Prism::ModelLoader::GetInstance()->LoadSprite(tankUnitPath, unitSize);
		myGruntPortrait = Prism::ModelLoader::GetInstance()->LoadSprite(gruntPortraitPath, portraitSize);
		myRangerPortrait = Prism::ModelLoader::GetInstance()->LoadSprite(rangerPortraitPath, portraitSize);
		myTankPortrait = Prism::ModelLoader::GetInstance()->LoadSprite(tankPortraitPath, portraitSize);
		myBuildingPortrait = Prism::ModelLoader::GetInstance()->LoadSprite(buildingPortraitPath, portraitSize);
		myStatsSprite = Prism::ModelLoader::GetInstance()->LoadSprite(statsSpritePath, statsSize);
		myUpgradeStar = Prism::ModelLoader::GetInstance()->LoadSprite(starSpritePath, starSize);

		int maxQueue = aPlayer->GetBuildingComponent().GetMaxQueue();
		//CU::Vector2<float> queuePosition = myPortraitPosition;
		//queuePosition.x += portraitSize.x;

		for (int i = 0; i < maxQueue; i++)
		{
			int index = i;

			CU::Vector2<float> position = abortPosition;
			ButtonWidget* abortButton = new ButtonWidget(aReader, abortButtonElement, aPlayer);

			if (i >= abortButtonsPerRow)
			{
				position.y -= abortButton->GetSize().y;
				index -= abortButtonsPerRow;
			}

			position += abortButton->GetSize() / 2.f;
			position.x += abortButton->GetSize().x * float(index);

			abortButton->SetPosition(position);
			abortButton->SetEvent(new OnClickMessage(eOnClickEvent::ABORT_QUEUE, i));
			myQueueButtons.Add(abortButton);
		}

		// has to be newed after abortbuttons:

		CU::Vector2<float> timerSize(myQueueButtons[0]->GetSize().x * float(abortButtonsPerRow), myQueueButtons[0]->GetSize().y / 2.f);

		myBuildingTimer = new BarWidget(myPlayerBuilding.GetMaxBuildTime(), myPlayerBuilding.GetCurrentBuildTime(), timerSize, PLAYER_COLOR);
		myActiveQueueOverlay = Prism::ModelLoader::GetInstance()->LoadSprite(activeQueueOverlaySpritePath
			, myQueueButtons[0]->GetSize(), myQueueButtons[0]->GetSize() / 2.f); 
	}

	UnitInfoWidget::~UnitInfoWidget()
	{
		SAFE_DELETE(myGruntUnit);
		SAFE_DELETE(myRangerUnit);
		SAFE_DELETE(myTankUnit);
		SAFE_DELETE(myGruntPortrait);
		SAFE_DELETE(myRangerPortrait);
		SAFE_DELETE(myTankPortrait);
		SAFE_DELETE(myBuildingPortrait);
		SAFE_DELETE(myStatsSprite);
		SAFE_DELETE(myBuildingTimer);
		SAFE_DELETE(myActiveQueueOverlay);
		SAFE_DELETE(myUpgradeStar);
		myQueueButtons.DeleteAll();
	}

	void UnitInfoWidget::Update(float)
	{
		myIsUnitSelected = myUnits.Size() > 0;
		if (myIsUnitSelected == true)
		{
			mySelectedType = myUnits[0]->GetType();
		}

		for (int i = myPlayerBuilding.GetQueue().Size(); i < myQueueButtons.Size(); i++)
		{
			myQueueButtons[i]->OnMouseExit(); // to prevent buttons getting stuck on hover image
		}
	}

	void UnitInfoWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsUnitSelected == true)
		{
			if (mySelectedType == eEntityType::UNIT)
			{
				if (myUnits.Size() > 1)
				{
					RenderUnitGroupInfo(aParentPosition);
				}
				else
				{
					RenderUnitInfo(aParentPosition);
				}
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				RenderBaseInfo(aParentPosition);
			}
		}
	}

	void UnitInfoWidget::OnMousePressed(const CU::Vector2<float>& aPosition)
	{
		if (myUnits.Size() > 1)
		{
			for (int i = 0; i < myUnits.Size(); i++)
			{

				int x = i % 4;
				int y = i / 4;

				CU::Vector2<float> position = { (myPosition.x + myGruntUnit->GetSize().x * x) + (x * 10.f), myPosition.y - myGruntUnit->GetSize().y * y - y * 10.f };

				//CU::Vector2<float> position = { (myGruntUnit->GetSize().x * i) + (i * 10.f), 0.f };
				position += myUnitPosition + myParent->GetPosition(); // hitta bättre lösning för detta

				if (aPosition.x >= position.x &&
					aPosition.y >= position.y &&
					aPosition.x <= position.x + myGruntUnit->GetSize().x &&
					aPosition.y <= position.y + myGruntUnit->GetSize().y)
				{
					PostMaster::GetInstance()->SendMessage(SelectUnitMessage(i));
				}
			}
		}
	}

	void UnitInfoWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myBuildingTimer->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myTextScale = (myTextScale / anOldWindowSize.x) * aNewWindowSize.x;

		for (int i = 0; i < myQueueButtons.Size(); i++)
		{
			myQueueButtons[i]->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		}

		myActiveQueueOverlay->SetSize(myQueueButtons[0]->GetSize(), myQueueButtons[0]->GetSize() / 2.f);

		if (myIsFullscreen == false)
		{
			CU::Vector2<float> unitRatioSize = myGruntUnit->GetSize() / anOldWindowSize.x;
			CU::Vector2<float> portraitRatioSize = myBuildingPortrait->GetSize() / anOldWindowSize.x;

			myGruntUnit->SetSize(unitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myRangerUnit->SetSize(unitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myTankUnit->SetSize(unitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myGruntPortrait->SetSize(portraitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myRangerPortrait->SetSize(portraitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myTankPortrait->SetSize(portraitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myBuildingPortrait->SetSize(portraitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myUnitPosition = (myUnitPosition / anOldWindowSize.x) * aNewWindowSize.x;
			myPortraitPosition = (myPortraitPosition / anOldWindowSize.x) * aNewWindowSize.x;
			myStatsSprite->SetSize((myStatsSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, { 0.f, 0.f });
			myTimerPosition = (myTimerPosition / anOldWindowSize.x) * aNewWindowSize.x;
			myUpgradeStarPosition = (myUpgradeStarPosition / anOldWindowSize.x) * aNewWindowSize.x;
			myUpgradeStar->SetSize((myUpgradeStar->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, { 0.f, 0.f });
		}
		else
		{
			CU::Vector2<float> unitRatioSize = myGruntUnit->GetSize() / anOldWindowSize;
			CU::Vector2<float> portraitRatioSize = myBuildingPortrait->GetSize() / anOldWindowSize;

			myGruntUnit->SetSize(unitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myRangerUnit->SetSize(unitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myTankUnit->SetSize(unitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myGruntPortrait->SetSize(portraitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myRangerPortrait->SetSize(portraitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myTankPortrait->SetSize(portraitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myBuildingPortrait->SetSize(portraitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myUnitPosition = (myUnitPosition / anOldWindowSize) * aNewWindowSize;
			myPortraitPosition = (myPortraitPosition / anOldWindowSize) * aNewWindowSize;
			myStatsSprite->SetSize((myStatsSprite->GetSize() / anOldWindowSize) * aNewWindowSize, { 0.f, 0.f });
			myTimerPosition = (myTimerPosition / anOldWindowSize) * aNewWindowSize;
			myUpgradeStarPosition = (myUpgradeStarPosition / anOldWindowSize) * aNewWindowSize;
			myUpgradeStar->SetSize((myUpgradeStar->GetSize() / anOldWindowSize) * aNewWindowSize, { 0.f, 0.f });
		}
	}

	Widget*	UnitInfoWidget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		const CU::GrowingArray<BuildInfo>& queue = myPlayerBuilding.GetQueue();

		for (int i = 0; i < queue.Size(); i++)
		{
			Widget* widget = myQueueButtons[i]->MouseIsOver(aPosition - myPosition);
			if (widget != nullptr)
			{
				return widget;
			}
		}

		return this;
	}

	void UnitInfoWidget::RenderBaseInfo(const CU::Vector2<float>& aParentPosition)
	{
		myBuildingPortrait->Render(myPosition + aParentPosition + myPortraitPosition);

		if (myPlayerBuilding.GetEntityToSpawn() != eEntityType::EMPTY)
		{
			myBuildingTimer->Update(0.f);
			//CU::Vector2<float> position = myPosition + aParentPosition + myUnitPosition + (myBuildingTimer->GetSize() / 2.f);
			myBuildingTimer->Render(myPosition + aParentPosition + myTimerPosition);
			//position.x += myBuildingTimer->GetSize().x + 10.f;
			//Prism::Engine::GetInstance()->PrintText(myBuilding.GetSpawnQueueSize(), position, Prism::eTextType::DEBUG_TEXT, myTextScale);

			const CU::GrowingArray<BuildInfo>& queue = myPlayerBuilding.GetQueue();
			CU::Vector2<float> portraitScale = myQueueButtons[0]->GetSize() / myGruntPortrait->GetSize();

			for (int i = 0; i < queue.Size(); i++)
			{
				myQueueButtons[i]->Render(myPosition + aParentPosition);

				CU::Vector2<float> portraitPosition = (myQueueButtons[i]->GetPosition() - 
					(myQueueButtons[i]->GetSize() / 2.f)) + (myPosition + aParentPosition);

				switch (queue[i].myUnit)
				{
				case eUnitType::GRUNT:
					myGruntPortrait->Render(portraitPosition, portraitScale);
					break;
				case eUnitType::RANGER:
					myRangerPortrait->Render(portraitPosition, portraitScale);
					break;
				case eUnitType::TANK:
					myTankPortrait->Render(portraitPosition, portraitScale);
					break;
				}

				if (i == 0)
				{
					myActiveQueueOverlay->Render(myQueueButtons[i]->GetPosition() + myPosition + aParentPosition);
				}
			}
		}
	}

	void UnitInfoWidget::RenderUnitInfo(const CU::Vector2<float>& aParentPosition)
	{
		if (myUnits[0]->GetType() != eEntityType::UNIT) return;
		CU::Vector2<float> portraitPosition = myPosition + aParentPosition + myPortraitPosition;
		CU::Vector2<float> upgradePosition = portraitPosition;

		CU::Vector4<float> color(1.f, 1.f, 1.f, 1.f);
		HealthComponent* toCheck = myUnits[0]->GetComponent<HealthComponent>();

		if (myUnits[0]->GetOwner() != eOwnerType::PLAYER)
		{
			color = myEnemyColor;
		}
		else if (toCheck->GetIsHealing() == true && toCheck->GetCurrentHealth() < toCheck->GetMaxHealth())
		{
			color = { 0.f, 1.f, 0.f, 1.f };
		}
		else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.3f)
		{
			color = { 1.f, 0.f, 0.f, 1.f };
		}
		else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.5f)
		{
			color = { 1.f, 0.4f, 0.4f, 1.f };
		}
		else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.7f)
		{
			color = { 1.f, 0.7f, 0.7f, 1.f };
		}

		if (myUnits[0]->GetUnitType() == eUnitType::GRUNT)
		{
			myGruntPortrait->Render(portraitPosition, { 1.f, 1.f }, color);
		}
		else if (myUnits[0]->GetUnitType() == eUnitType::RANGER)
		{
			myRangerPortrait->Render(portraitPosition, { 1.f, 1.f }, color);
		}
		else
		{
			myTankPortrait->Render(portraitPosition, { 1.f, 1.f }, color);
		}

		upgradePosition.x += myGruntPortrait->GetSize().y * 0.9f;

		myUnits[0]->GetComponent<PromotionComponent>()->RenderPromotion(portraitPosition, { 1.f, 1.f }, myUnits[0]->GetOwner() == eOwnerType::PLAYER ? CU::Vector4<float>(1.f, 1.f, 1.f, 1.f) : myEnemyColor);

		if (myUnits[0]->GetUnitType() != eUnitType::NON_ATTACK_TUTORIAL)
		{
			CU::Vector4<float> color = { 1.f, 1.f, 1.f, 1.f };
			int upgradeLevel = 0;

			if (myUnits[0]->GetOwner() == eOwnerType::PLAYER)
			{
				upgradeLevel = myPlayerBuilding.GetUpgradeLevel(myUnits[0]->GetUnitType());
			}
			else if (myUnits[0]->GetOwner() == eOwnerType::ENEMY)
			{
				upgradeLevel = myEnemyBuilding.GetUpgradeLevel(myUnits[0]->GetUnitType());
				color = myEnemyColor;
			}

			for (; upgradeLevel > 0; --upgradeLevel)
			{
				CU::Vector2<float> starPosition = myPosition + aParentPosition + myUpgradeStarPosition;
				starPosition.x += myUpgradeStar->GetSize().x * float(upgradeLevel - 1);
				myUpgradeStar->Render(starPosition, { 1.f, 1.f }, color);
			}
		}

		portraitPosition.y -= myGruntPortrait->GetSize().y * 0.32f;
		//portraitPosition.x += myGruntPortrait->GetSize().x / 3.f;

		std::string currentHealth = std::to_string(int(myUnits[0]->GetComponent<HealthComponent>()->GetCurrentHealth()));
		//Prism::Engine::GetInstance()->PrintText(currentHealth, portraitPosition, Prism::eTextType::RELEASE_TEXT, 1.f, color);

		//std::string maxHealth = "/" + std::to_string(int(myUnits[0]->GetComponent<HealthComponent>()->GetMaxHealth()));

		currentHealth.append("/" + std::to_string(int(myUnits[0]->GetComponent<HealthComponent>()->GetMaxHealth())));
		Prism::Engine::GetInstance()->PrintText(currentHealth, { portraitPosition.x, portraitPosition.y }
		, Prism::eTextType::RELEASE_TEXT, 1.f, color);



		color.x = 1.f;
		color.y = 1.f;
		color.z = 1.f;
		color.w = 1.f;

		if (myUnits[0]->GetOwner() != eOwnerType::PLAYER)
		{
			color = myEnemyColor;
		}
		else if (myUnits[0]->GetUnitType() == eUnitType::TANK)
		{
			if (myUnits[0]->GetComponent<EnrageComponent>()->IsActive() == true)
			{
				color.x = 0.f;
				color.z = 0.f;
			}
		}

		CU::Vector2<float> position = { myGruntUnit->GetSize().x / 2.f, myPosition.y };
		position += aParentPosition + myUnitPosition;
		myStatsSprite->Render(position, { 1.f, 1.f }, myUnits[0]->GetOwner() == eOwnerType::PLAYER ?
			CU::Vector4<float>(1.f, 1.f, 1.f, 1.f) : myEnemyColor);

		position.x += myStatsSprite->GetSize().x * 0.025f;
		position.y -= myStatsSprite->GetSize().y * 0.5f;
		Prism::Engine::GetInstance()->PrintText(myUnits[0]->GetComponent<HealthComponent>()->GetArmor()
			, position, Prism::eTextType::RELEASE_TEXT, 1.f, color);

		position.x += myStatsSprite->GetSize().x * 0.17f;
		Prism::Engine::GetInstance()->PrintText(myUnits[0]->GetComponent<ActorComponent>()->GetAttackDamage()
			, position, Prism::eTextType::RELEASE_TEXT, 1.f, color);

		position.x += myStatsSprite->GetSize().x * 0.17f;
		Prism::Engine::GetInstance()->PrintText(myUnits[0]->GetMaxSpeed()
			, position, Prism::eTextType::RELEASE_TEXT, 1.f, color);

		position.x += myStatsSprite->GetSize().x * 0.17f;
		Prism::Engine::GetInstance()->PrintText(myUnits[0]->GetComponent<ActorComponent>()->GetAttackSpeed()
			, position, Prism::eTextType::RELEASE_TEXT, 1.f, color);
	}

	void UnitInfoWidget::RenderUnitGroupInfo(const CU::Vector2<float>& aParentPosition)
	{
		for (int i = 0; i < myUnits.Size(); i++)
		{
			Prism::SpriteProxy* portrait = nullptr;

			if (myUnits[i]->GetUnitType() == eUnitType::GRUNT)
			{
				portrait = myGruntUnit;
			}
			else if (myUnits[i]->GetUnitType() == eUnitType::RANGER)
			{
				portrait = myRangerUnit;
			}
			else
			{
				portrait = myTankUnit;
			}

			CU::Vector4<float> color(1.f, 1.f, 1.f, 1.f);
			HealthComponent* toCheck = myUnits[i]->GetComponent<HealthComponent>();
			if (toCheck->GetIsHealing() == true && toCheck->GetCurrentHealth() < toCheck->GetMaxHealth())
			{
				color = { 0.f, 1.f, 0.f, 1.f };
			}
			else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.3f)
			{
				color = { 1.f, 0.f, 0.f, 1.f };
			}
			else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.5f)
			{
				color = { 1.f, 0.4f, 0.4f, 1.f };
			}
			else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.7f)
			{
				color = { 1.f, 0.7f, 0.7f, 1.f };
			}

			int x = i % 4;
			int y = i / 4;

			CU::Vector2<float> position = { (myPosition.x + portrait->GetSize().x * x) + (x * 10.f), myPosition.y - portrait->GetSize().y * y - y * 10.f };
			position += aParentPosition + myUnitPosition;

			//CU::Vector2<float> upgradePosition = position;
			//upgradePosition.x += portrait->GetSize().x * 0.7f;
			//upgradePosition.y += portrait->GetSize().y * 0.15f;

			portrait->Render(position, { 1.f, 1.f }, color);
			position += portrait->GetSize() * 0.2f;
			myUnits[i]->GetComponent<PromotionComponent>()->RenderPromotion(position, { 0.7f, 0.7f });
			//Prism::Engine::GetInstance()->PrintText(myBuilding.GetUpgradeLevel(myUnits[i]->GetUnitType()), upgradePosition, Prism::eTextType::RELEASE_TEXT, 0.8f);
		}
	}
}