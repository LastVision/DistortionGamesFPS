#pragma once

namespace CU
{
	class InputWrapper;
}

class Entity;

namespace Prism
{
	class Camera;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class WidgetContainer;
	class Widget;
	class Cursor;

	class GUIManager
	{
	public:
		GUIManager(Cursor* aCursor, const std::string& aXMLPath, const Prism::Camera* aCamera, int aLeveID);
		~GUIManager();

		void AddWidget(Widget* aWidget);
		void Update(float aDelta);
		void Render();

		void OnResize(int aWidth, int aHeight);

		CU::Vector3<float> CalcCameraMovement() const;

		bool MouseOverGUI();

		void ReadXML(const std::string& aXMLPath);

		const Widget* GetActiveWidget() const;

		void SetPosition(const CU::Vector2<float>& aPosition);

		WidgetContainer* GetWidgetContainer();

		void SetButtonText(int aButtonId, const std::string& aText);

		void SetMouseShouldRender(bool aShouldRender);

	private:
		void ReadContainers(XMLReader& aReader, tinyxml2::XMLElement* aContainerElement);
		void ReadFiles(XMLReader& aReader, tinyxml2::XMLElement* aFilePathElement);

		void CheckMousePressed();
		void CheckMouseDown();
		void CheckMouseReleased();
		void CheckMouseMoved();
		void CheckMouseEntered();
		void CheckMouseExited();

		WidgetContainer* myWidgets;

		const Prism::Camera* myCamera;

		Widget* myActiveWidget;
		Cursor* myCursor;

		CU::Vector2<float> myMousePosition;
		CU::Vector2<float> myWindowSize;

		int myLevelID;
	};

	inline const Widget* GUIManager::GetActiveWidget() const
	{
		return myActiveWidget;
	}

	inline WidgetContainer* GUIManager::GetWidgetContainer()
	{
		return myWidgets;
	}
}