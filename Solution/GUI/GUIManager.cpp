#include "stdafx.h"
#include "AbilityButton.h"
#include "ButtonWidget.h"
#include "ControlGroupWidget.h"
#include "Cursor.h"
#include <Engine.h>
#include "../Game/PlayerDirector.h"
#include "GUIManager.h"
#include "../InputWrapper/InputWrapper.h"
#include "MiniMapWidget.h"
#include "ResourceBarWidget.h"
#include "SpriteWidget.h"
#include "TextWidget.h"
#include "TooltipWidget.h"
#include "UnitActionWidget.h"
#include "UnitCapWidget.h"
#include "UnitInfoWidget.h"
#include "UpgradeButtonWidget.h"
#include "WidgetContainer.h"

namespace GUI
{
	GUIManager::GUIManager(Cursor* aCursor, const std::string& aXMLPath, const PlayerDirector* aPlayer
		, const AIDirector* anAI, const Prism::Camera* aCamera, int aLeveID)
		: myActiveWidget(nullptr)
		, myCursor(aCursor)
		, myPlayer(aPlayer)
		, myAI(anAI)
		, myMousePosition({ 0.f, 0.f })
		, myCamera(aCamera)
		, myLevelID(aLeveID)
	{
		myWindowSize = { 1920.f, 1080.f }; // XML coordinates respond to this resolution, will be resized

		myWidgets = new WidgetContainer(nullptr, nullptr, myWindowSize, true);
		myWidgets->SetPosition({ 0.f, 0.f });

		ReadXML(aXMLPath);

		OnResize(Prism::Engine::GetInstance()->GetWindowSizeInt().x, Prism::Engine::GetInstance()->GetWindowSizeInt().y);
	}

	GUIManager::~GUIManager()
	{
		SAFE_DELETE(myWidgets);
		myActiveWidget = nullptr;
	}

	void GUIManager::AddWidget(Widget* aWidget)
	{
		myWidgets->AddWidget(aWidget);
	}

	void GUIManager::Update(float aDelta)
	{
		myMousePosition = myCursor->GetMousePosition();

		myWidgets->Update(aDelta);

		CheckMouseMoved();
		CheckMouseExited();
		CheckMouseDown();
		CheckMousePressed();
		CheckMouseReleased();

		CheckMouseEntered();
	}

	void GUIManager::Render()
	{
		myWidgets->Render({ 0.f, 0.f });
	}

	void GUIManager::OnResize(int aWidth, int aHeight)
	{
		CU::Vector2<float> newSize = { float(aWidth), float(aHeight) };
		myWidgets->OnResize(newSize, myWindowSize, true);
		myWindowSize = newSize;
	}

	CU::Vector3<float> GUIManager::CalcCameraMovement() const
	{
		CU::Vector3<float> movement;

		float epsilon = 0.01f;
		if (myCursor->GetMousePositionZeroToOne().x < epsilon)
		{
			movement.x = -1.f;
		}
		if (myCursor->GetMousePositionZeroToOne().x > 1.f - epsilon)
		{
			movement.x = 1.f;
		}

		if (myCursor->GetMousePositionZeroToOne().y < epsilon)
		{
			movement.z = -1.f;
		}
		if (myCursor->GetMousePositionZeroToOne().y > 1.f - epsilon)
		{
			movement.z = 1.f;
		}

		return movement;
	}

	bool GUIManager::MouseOverGUI()
	{
		return myActiveWidget != nullptr && myActiveWidget != myWidgets;
	}

	void GUIManager::ReadXML(const std::string& aXMLPath)
	{
		XMLReader reader;
		reader.OpenDocument(aXMLPath);

		tinyxml2::XMLElement* rootElement = reader.FindFirstChild("root");
		tinyxml2::XMLElement* containerElement = reader.FindFirstChild(rootElement, "container");
		tinyxml2::XMLElement* XMLPathElement = reader.FindFirstChild(rootElement, "file");

		ReadContainers(reader, containerElement);
		ReadFiles(reader, XMLPathElement);

		reader.CloseDocument();
	}

	void GUIManager::SetPosition(const CU::Vector2<float>& aPosition)
	{
		myWidgets->SetPosition(aPosition);
	}

	void GUIManager::ReadContainers(XMLReader& aReader, tinyxml2::XMLElement* aContainerElement)
	{
		std::string path = "";
		CU::Vector2<float> size;
		CU::Vector2<float> position;

		for (; aContainerElement != nullptr; aContainerElement = aReader.FindNextElement(aContainerElement, "container"))
		{
			bool isFullscreen = false;
			bool isClickable = true;
			bool isScrolling = false;
			Prism::SpriteProxy* backgroundSprite = nullptr;
			Prism::SpriteProxy* vignetteSprite = nullptr;

			aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aContainerElement, "size"), "x", size.x);
			aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aContainerElement, "size"), "y", size.y);
			aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aContainerElement, "position"), "x", position.x);
			aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aContainerElement, "position"), "y", position.y);

			tinyxml2::XMLElement* spriteElement = aReader.FindFirstChild(aContainerElement, "backgroundsprite");
			tinyxml2::XMLElement* spriteSizeElement = aReader.FindFirstChild(aContainerElement, "backgroundsize");
			tinyxml2::XMLElement* fullscreenElement = aReader.FindFirstChild(aContainerElement, "isfullscreen");
			tinyxml2::XMLElement* clickableElement = aReader.FindFirstChild(aContainerElement, "isclickable");
			tinyxml2::XMLElement* scrollingElement = aReader.FindFirstChild(aContainerElement, "isscrolling");
			tinyxml2::XMLElement* vignetteElement = aReader.FindFirstChild(aContainerElement, "vignettesprite");

			if (spriteElement != nullptr)
			{
				aReader.ForceReadAttribute(spriteElement, "path", path);

				if (scrollingElement != nullptr)
				{
					aReader.ForceReadAttribute(scrollingElement, "value", isScrolling);
				}
				if (spriteSizeElement != nullptr)
				{
					CU::Vector2<float> spriteSize;
					aReader.ForceReadAttribute(spriteSizeElement, "x", spriteSize.x);
					aReader.ForceReadAttribute(spriteSizeElement, "y", spriteSize.y);
					if (isScrolling == true)
					{
						spriteSize.x = spriteSize.y * 2.f;
					}
					backgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(path, spriteSize);
				}
				else
				{
					backgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(path, size);
				}
			}

			if (vignetteElement != nullptr)
			{
				aReader.ForceReadAttribute(vignetteElement, "path", path);

				vignetteSprite = Prism::ModelLoader::GetInstance()->LoadSprite(path, size);
			}

			if (fullscreenElement != nullptr)
			{
				aReader.ForceReadAttribute(fullscreenElement, "value", isFullscreen);
			}

			if (clickableElement != nullptr)
			{
				aReader.ForceReadAttribute(clickableElement, "value", isClickable);
			}

			GUI::WidgetContainer* container = new WidgetContainer(backgroundSprite, vignetteSprite, size, isFullscreen, isScrolling);
			container->SetPosition(position);

			tinyxml2::XMLElement* widgetElement = aReader.FindFirstChild(aContainerElement, "widget");
			for (; widgetElement != nullptr; widgetElement = aReader.FindNextElement(widgetElement))
			{
				std::string type = "";

				aReader.ForceReadAttribute(widgetElement, "type", type);

				if (type == "button")
				{
					ButtonWidget* button = new ButtonWidget(&aReader, widgetElement, myPlayer);
					container->AddWidget(button);
				}
				else if (type == "unit_info")
				{
					UnitInfoWidget* unitInfo = new UnitInfoWidget(&aReader, widgetElement, myPlayer, myAI);
					container->AddWidget(unitInfo);
				}
				else if (type == "unit_action")
				{
					UnitActionWidget* unitActions = new UnitActionWidget(&aReader, widgetElement, myPlayer->GetSelectedUnits(), myPlayer);
					container->AddWidget(unitActions);
				}
				else if (type == "minimap")
				{
					MiniMapWidget* minimap = new MiniMapWidget(&aReader, widgetElement, myCamera, myPlayer->GetRenderDragSelection(), myLevelID);
					container->AddWidget(minimap);
				}
				else if (type == "resourcebar")
				{
					ResourceBarWidget* resourceBar = new ResourceBarWidget(&aReader, widgetElement, myPlayer, myAI);
					container->AddWidget(resourceBar);
				}
				else if (type == "control_groups")
				{
					ControlGroupWidget* controlGroup = new ControlGroupWidget(&aReader, widgetElement, myPlayer, size);
					container->AddWidget(controlGroup);
				}
				else if (type == "text")
				{
					TextWidget* textWidget = new TextWidget(&aReader, widgetElement);
					container->AddWidget(textWidget);
				}
				else if (type == "unit_cap")
				{
					UnitCapWidget* unitCapWidget = new UnitCapWidget(&aReader, widgetElement, size, myPlayer->GetUnitCap()
						, myPlayer->GetUnitCount());
					container->AddWidget(unitCapWidget);
				}
				else if (type == "upgrade_button")
				{
					UpgradeButtonWidget* upgradeButtonWidget = new UpgradeButtonWidget(&aReader, widgetElement, myPlayer);
					container->AddWidget(upgradeButtonWidget);
				}
				else if (type == "ability_button")
				{
					AbilityButton* upgradeButtonWidget = new AbilityButton(&aReader, widgetElement, myPlayer);
					container->AddWidget(upgradeButtonWidget);
				}
				else if (type == "tool_tip")
				{
					TooltipWidget* tooltipWidget = new TooltipWidget(&aReader, widgetElement, this, myPlayer);
					container->AddWidget(tooltipWidget);
				}
				else if (type == "sprite")
				{
					SpriteWidget* spriteWidget = new SpriteWidget(&aReader, widgetElement);
					container->AddWidget(spriteWidget);
				}
				else
				{
					// should assert when all widgets are in
				}
			}
			container->SetIsClickable(isClickable);
			myWidgets->AddWidget(container);
		}
	}

	void GUIManager::ReadFiles(XMLReader& aReader, tinyxml2::XMLElement* aFilePathElement)
	{
		for (; aFilePathElement != nullptr; aFilePathElement = aReader.FindNextElement(aFilePathElement, "file"))
		{
			std::string filePath = "";

			aReader.ForceReadAttribute(aFilePathElement, "path", filePath);

			ReadXML(filePath);
		}
	}

	void GUIManager::CheckMousePressed()
	{
		if (myActiveWidget != nullptr)
		{
			if (CU::InputWrapper::GetInstance()->MouseIsPressed(0) == true)
			{
				myActiveWidget->OnMousePressed(myMousePosition);
			}
			if (CU::InputWrapper::GetInstance()->MouseIsPressed(1) == true)
			{
				myActiveWidget->OnRightMousePressed(myMousePosition);
			}
		}
	}

	void GUIManager::CheckMouseDown()
	{
		if (myActiveWidget != nullptr)
		{
			if (CU::InputWrapper::GetInstance()->MouseDown(0) == true)
			{
				myActiveWidget->OnMouseDown(myMousePosition);
			}
			if (CU::InputWrapper::GetInstance()->MouseDown(1) == true)
			{
				myActiveWidget->OnRightMouseDown(myMousePosition);
			}
		}
	}

	void GUIManager::CheckMouseReleased()
	{
		if (myActiveWidget != nullptr)
		{
			if (CU::InputWrapper::GetInstance()->MouseUp(0) == true)
			{
				myActiveWidget->OnMouseUp();
				myActiveWidget->OnMouseEnter(); // hover button again after pressing it
			}
		}
	}

	void GUIManager::CheckMouseMoved()
	{
		if (myActiveWidget != nullptr)
		{
			if (myMousePosition.x != 0 || myMousePosition.y != 0)
			{
				myActiveWidget->OnMouseMoved(myMousePosition);
			}
		}
	}

	void GUIManager::CheckMouseEntered()
	{
		Widget* activeWidget = myWidgets->MouseIsOver(myMousePosition);

		if (activeWidget != nullptr && activeWidget->IsClickable() == false)
		{
			return;
		}

		if (activeWidget != nullptr && activeWidget != myActiveWidget)
		{
			activeWidget->OnMouseEnter();
			myActiveWidget = activeWidget;
		}
	}

	void GUIManager::CheckMouseExited()
	{
		Widget* activeWidget = myWidgets->MouseIsOver(myMousePosition);

		if (myActiveWidget != activeWidget)
		{
			if (myActiveWidget != nullptr)
			{
				myActiveWidget->OnMouseExit();
				myActiveWidget = nullptr;
			}
		}
	}
}