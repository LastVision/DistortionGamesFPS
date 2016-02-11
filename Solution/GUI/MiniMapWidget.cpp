#include "stdafx.h"

#include <AudioInterface.h>
#include "../Game/GameDefines.h"
#include <EventPositionMessage.h>
#include "../Engine/Camera.h"
#include "../Entity/Entity.h"
#include "../Entity/PollingStation.h"
#include "MiniMapWidget.h"
#include <MinimapEventMessage.h>
#include <MinimapMoveMessage.h>
#include <MoveCameraMessage.h>
#include <PostMaster.h>
#include "../Entity/TriggerComponent.h"
#include "../Game/FogOfWarMap.h"

namespace GUI
{
	MiniMapWidget::MiniMapWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const Prism::Camera* aCamera
		, const bool& aCantClickOn, int aLevelID)
		: Widget()
		, myCantClickOn(aCantClickOn)
		, myEventSprites(8)
		, myCurrentEventSprite(0)
	{
		std::string mapPath = "";
		std::string unitPath = "";
		std::string basePath = "";
		std::string resourcePath = "";
		std::string victoryPath = "";
		std::string artifactPath = "";
		std::string eventPath = "";
		std::string cameraPath = "";
		std::string levelID = "tutorial";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		if (aLevelID == 0)
		{
			levelID = "tutorial";
		}
		else if (aLevelID == 1)
		{
			levelID = "first";
		}

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "mapsprites"), levelID, mapPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unit"), "sprite", unitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "base"), "sprite", basePath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "resourcepoints"), "sprite", resourcePath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "victorypoints"), "sprite", victoryPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "artifacts"), "sprite", artifactPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "event"), "sprite", eventPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "camera"), "sprite", cameraPath);

		myCameraOrientation = &aCamera->GetOrientation();

		myPlaceholderSprite = Prism::ModelLoader::GetInstance()->LoadSprite(mapPath, mySize);
		myUnitSprite = Prism::ModelLoader::GetInstance()->LoadSprite(unitPath, { 10.f, 10.f }, { 5.f, 5.f });
		myBaseSprite = Prism::ModelLoader::GetInstance()->LoadSprite(basePath, { 20.f, 20.f }, { 10.f, 10.f });
		myResourcePointSprite = Prism::ModelLoader::GetInstance()->LoadSprite(resourcePath, { 20.f, 20.f }, { 10.f, 10.f });
		myVictoryPointSprite = Prism::ModelLoader::GetInstance()->LoadSprite(victoryPath, { 20.f, 20.f }, { 10.f, 10.f });
		myArtifactSprite = Prism::ModelLoader::GetInstance()->LoadSprite(artifactPath, { 15.f, 15.f }, { 7.5f, 7.5f });
		myEventSprite = Prism::ModelLoader::GetInstance()->LoadSprite(eventPath, { 20.f, 20.f }, { 10.f, 10.f });

		CU::Vector2<float> cameraSize = { mySize.x / 2.f, mySize.y / 4.f };

		myCameraFrustum = Prism::ModelLoader::GetInstance()->LoadSprite(cameraPath, cameraSize, cameraSize / 2.f);

		PostMaster::GetInstance()->Subscribe(eMessageType::MINIMAP_EVENT, this);

		for (int i = 0; i < 8; ++i)
		{
			MiniMapEvent toAdd;
			toAdd.myEventPosition = { 128.f, 128.f };
			toAdd.myEventSprite = myEventSprite;
			toAdd.myEventTime = 3.f;
			toAdd.myEventTimer = 0.f;
			toAdd.myShouldRenderEvent = false;
			myEventSprites.Add(toAdd);
		}
	}

	MiniMapWidget::~MiniMapWidget()
	{
		PostMaster::GetInstance()->UnSubscribe(eMessageType::MINIMAP_EVENT, this);
		SAFE_DELETE(myPlaceholderSprite);
		SAFE_DELETE(myUnitSprite);
		SAFE_DELETE(myBaseSprite);
		SAFE_DELETE(myResourcePointSprite);
		SAFE_DELETE(myVictoryPointSprite);
		SAFE_DELETE(myArtifactSprite);
		SAFE_DELETE(myCameraFrustum);
		SAFE_DELETE(myEventSprite);
	}

	void MiniMapWidget::Update(float aDelta)
	{
		for (int i = 0; i < myEventSprites.Size(); ++i)
		{
			if (myEventSprites[i].myShouldRenderEvent == true)
			{
				myEventSprites[i].myEventTimer += aDelta;
			}
			if (myEventSprites[i].myEventTimer > myEventSprites[i].myEventTime)
			{
				myEventSprites[i].myShouldRenderEvent = false;
			}
		}
	}

	void MiniMapWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myPlaceholderSprite->Render(aParentPosition + myPosition);

		RenderArtifacts(aParentPosition);
		RenderResourcePoints(aParentPosition);
		RenderVictoryPoints(aParentPosition);
		RenderBases(aParentPosition);
		RenderUnits(aParentPosition);

		CU::Vector2<float> cameraPosition = { myCameraOrientation->GetPos().x, myCameraOrientation->GetPos().z };
		cameraPosition /= 255.f;
		cameraPosition *= mySize;
		cameraPosition.x += myCameraFrustum->GetSize().x / 4.f;
		cameraPosition.y += myCameraFrustum->GetSize().y * 1.4f;
		myCameraFrustum->Render(cameraPosition);

		//if (myShouldRenderEvent == true)
		//{
		//	//if (static_cast<int>(myEventTimer) % 2 == 0)
		//	{
		//		CU::Vector2<float> position = (myEventPosition / 255.f) * mySize;
		//		float scale = 2 + 5 * log(myEventTimer + 1);
		//		myEventSprite->Render(aParentPosition + myPosition + position, { scale, scale }, { 1.f, 1.f, 1.f, 1.f - (myEventTimer * 51.f / 255.f) });
		//	}
		//}

		for (int i = 0; i < myEventSprites.Size(); ++i)
		{
			if (myEventSprites[i].myShouldRenderEvent == true)
			{

				CU::Vector2<float> position = (myEventSprites[i].myEventPosition / 255.f) * mySize;
				float scale = 1.f + 5 * log(myEventSprites[i].myEventTimer + 1.f);
				myEventSprites[i].myEventSprite->Render(aParentPosition + myPosition + position
					, { scale, scale }, { 1.f, 1.f, 1.f, 1.f - (myEventSprites[i].myEventTimer * 85.f / 255.f) });

			}
		}
	}

	void MiniMapWidget::OnMousePressed(const CU::Vector2<float>& aPosition)
	{
		if (myCantClickOn == false)
		{
			CU::Vector2<float> position = aPosition - myPosition;
			position /= mySize;
			PostMaster::GetInstance()->SendMessage(MoveCameraMessage(position, eHowToHandleMovement::ZERO_TO_ONE));
		}
	}

	void MiniMapWidget::OnRightMouseDown(const CU::Vector2<float>& aPosition)
	{
		if (myCantClickOn == false)
		{
			CU::Vector2<float> position = aPosition - myPosition;
			position /= mySize;
			PostMaster::GetInstance()->SendMessage(MinimapMoveMessage(position));
		}
	}

	void MiniMapWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);

		CU::Vector2<float> frustumRatio = myCameraFrustum->GetSize() / anOldWindowSize.x;

		myPlaceholderSprite->SetSize(mySize, { 0.f, 0.f });

		if (myIsFullscreen == false)
		{
			myCameraFrustum->SetSize(frustumRatio * aNewWindowSize.x, (frustumRatio * aNewWindowSize.x) / 2.f);
			myArtifactSprite->SetSize((myArtifactSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, ((myArtifactSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x) / 2.f);
			myBaseSprite->SetSize((myBaseSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, ((myBaseSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x) / 2.f);
			myResourcePointSprite->SetSize((myResourcePointSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, ((myResourcePointSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x) / 2.f);
			myVictoryPointSprite->SetSize((myVictoryPointSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, ((myVictoryPointSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x) / 2.f);
			myUnitSprite->SetSize((myUnitSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, ((myUnitSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x) / 2.f);
			myEventSprite->SetSize((myEventSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, ((myEventSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x) / 2.f);
		}
		else
		{
			myCameraFrustum->SetSize(frustumRatio * aNewWindowSize, (frustumRatio * aNewWindowSize) / 2.f);
			myArtifactSprite->SetSize((myArtifactSprite->GetSize() / anOldWindowSize) * aNewWindowSize, ((myArtifactSprite->GetSize() / anOldWindowSize) * aNewWindowSize) / 2.f);
			myBaseSprite->SetSize((myBaseSprite->GetSize() / anOldWindowSize) * aNewWindowSize, ((myBaseSprite->GetSize() / anOldWindowSize) * aNewWindowSize) / 2.f);
			myResourcePointSprite->SetSize((myResourcePointSprite->GetSize() / anOldWindowSize) * aNewWindowSize, ((myResourcePointSprite->GetSize() / anOldWindowSize) * aNewWindowSize) / 2.f);
			myVictoryPointSprite->SetSize((myVictoryPointSprite->GetSize() / anOldWindowSize) * aNewWindowSize, ((myVictoryPointSprite->GetSize() / anOldWindowSize) * aNewWindowSize) / 2.f);
			myUnitSprite->SetSize((myUnitSprite->GetSize() / anOldWindowSize) * aNewWindowSize, ((myUnitSprite->GetSize() / anOldWindowSize) * aNewWindowSize) / 2.f);
			myEventSprite->SetSize((myEventSprite->GetSize() / anOldWindowSize) * aNewWindowSize, ((myEventSprite->GetSize() / anOldWindowSize) * aNewWindowSize) / 2.f);
		}
	}

	void MiniMapWidget::ReceiveMessage(const MinimapEventMessage& aMessage)
	{
		//aMessage.myMessageType;
		//myEventPosition = aMessage.myPosition;
		//myEventTimer = 0.f;
		//myShouldRenderEvent = true;

		for (int i = 0; i < myEventSprites.Size(); ++i)
		{
			if (myEventSprites[i].myEventPosition == aMessage.myPosition)
			{
				return;
			}
		}
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("PLay_Ping", 0);
		myEventSprites[myCurrentEventSprite].myEventPosition = aMessage.myPosition;
		myEventSprites[myCurrentEventSprite].myEventTimer = 0.f;
		myEventSprites[myCurrentEventSprite].myShouldRenderEvent = true;

		PostMaster::GetInstance()->SendMessage(EventPositionMessage(myEventSprites[myCurrentEventSprite].myEventPosition));


		++myCurrentEventSprite;

		if (myCurrentEventSprite >= myEventSprites.Size())
		{
			myCurrentEventSprite = 0;
		}
	}

	void MiniMapWidget::RenderUnits(const CU::Vector2<float>& aParentPosition)
	{
		const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
		const CU::GrowingArray<Entity*>& enemyUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY);
		const CU::GrowingArray<Entity*>& neutralUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL);

		for (int i = 0; i < playerUnits.Size(); i++)
		{
			CU::Vector4<float> color = PLAYER_COLOR;
			if (playerUnits[i]->IsSelected() == true)
			{
				color = PLAYER_COLOR;
				color += CU::Vector4<float>(0.3f, 0.25f, 0.1f, 0.f);
			}
			CU::Vector2<float> position = (playerUnits[i]->GetPosition() / 255.f) * mySize;
			myUnitSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, color);
		}

		for (int i = 0; i < enemyUnits.Size(); i++)
		{
			if (FogOfWarMap::GetInstance()->IsVisible(enemyUnits[i]->GetPosition()))
			{
				CU::Vector2<float> position = (enemyUnits[i]->GetPosition() / 255.f) * mySize;
				myUnitSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, { 1.f, 0.f, 0.f, 1.f });
			}
		}

		for (int i = 0; i < neutralUnits.Size(); i++)
		{
			if (FogOfWarMap::GetInstance()->IsVisible(neutralUnits[i]->GetPosition()))
			{
				CU::Vector2<float> position = (neutralUnits[i]->GetPosition() / 255.f) * mySize;
				myUnitSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, { 0.5f, 0.5f, 0.f, 1.f });
			}
		}
	}

	void MiniMapWidget::RenderVictoryPoints(const CU::Vector2<float>& aParentPosition)
	{
		const CU::GrowingArray<Entity*>& victoryPoints = PollingStation::GetInstance()->GetVictoryPoints();

		for (int i = 0; i < victoryPoints.Size(); i++)
		{
			CU::Vector2<float> position = (victoryPoints[i]->GetPosition() / 255.f) * mySize;
			CU::Vector4<float> color = { 0.5f, 0.5f, 0.f, 1.f };

			if (victoryPoints[i]->GetTemporaryOwner() == eOwnerType::PLAYER)
			{
				color = PLAYER_COLOR;
			}
			else if (victoryPoints[i]->GetTemporaryOwner() == eOwnerType::ENEMY)
			{
				color = { 1.f, 0.f, 0.f, 1.f };
			}

			myVictoryPointSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, color);
		}
	}

	void MiniMapWidget::RenderResourcePoints(const CU::Vector2<float>& aParentPosition)
	{
		const CU::GrowingArray<Entity*>& resourcePoints = PollingStation::GetInstance()->GetResourcePoints();

		for (int i = 0; i < resourcePoints.Size(); i++)
		{
			CU::Vector2<float> position = (resourcePoints[i]->GetPosition() / 255.f) * mySize;
			CU::Vector4<float> color = { 0.5f, 0.5f, 0.f, 1.f };

			if (resourcePoints[i]->GetComponent<TriggerComponent>() != nullptr)
			{
				if (resourcePoints[i]->GetTemporaryOwner() == eOwnerType::PLAYER)
				{
					color = PLAYER_COLOR;
				}
				else if (resourcePoints[i]->GetTemporaryOwner() == eOwnerType::ENEMY)
				{
					color = { 1.f, 0.f, 0.f, 1.f };
				}

				myResourcePointSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, color);
			}
		}
	}

	void MiniMapWidget::RenderBases(const CU::Vector2<float>& aParentPosition)
	{
		const Entity* enemyBase = PollingStation::GetInstance()->GetBase(eOwnerType::ENEMY);
		const Entity* playerBase = PollingStation::GetInstance()->GetBase(eOwnerType::PLAYER);

		CU::Vector2<float> basePosition = (enemyBase->GetPosition() / 255.f) * mySize;
		myBaseSprite->Render(aParentPosition + myPosition + basePosition, { 1.f, 1.f }, { 1.f, 0.f, 0.f, 1.f });

		basePosition = (playerBase->GetPosition() / 255.f) * mySize;
		myBaseSprite->Render(aParentPosition + myPosition + basePosition, { 1.f, 1.f }, PLAYER_COLOR);
	}

	void MiniMapWidget::RenderArtifacts(const CU::Vector2<float>& aParentPosition)
	{
		const CU::GrowingArray<Entity*>& artifacts = PollingStation::GetInstance()->GetArtifacts();

		for (int i = 0; i < artifacts.Size(); i++)
		{
			CU::Vector2<float> position = (artifacts[i]->GetPosition() / 255.f) * mySize;

			myArtifactSprite->Render(aParentPosition + myPosition + position);
		}
	}
}