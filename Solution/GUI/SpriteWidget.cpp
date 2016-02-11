#include "stdafx.h"
#include "SpriteWidget.h"

namespace GUI
{
	SpriteWidget::SpriteWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: Widget()
		, myBackground(nullptr)
	{
		std::string spritePath = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "background"), "path", spritePath);

		tinyxml2::XMLElement* fullscreenElement = aReader->FindFirstChild(anXMLElement, "isfullscreen");

		if (fullscreenElement != nullptr)
		{
			aReader->ForceReadAttribute(fullscreenElement, "value", myIsFullscreen);
		}

		myBackground = Prism::ModelLoader::GetInstance()->LoadSprite(spritePath, mySize);
	}

	SpriteWidget::~SpriteWidget()
	{
		SAFE_DELETE(myBackground);
	}

	void SpriteWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myBackground->Render(myPosition + aParentPosition);
	}

	void SpriteWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewSize, anOldSize, aIsFullScreen);

		if (myIsFullscreen == false)
		{
			CU::Vector2<float> ratio = myBackground->GetSize() / anOldSize.x;
			myBackground->SetSize(aNewSize.x * ratio, { 0.f, 0.f });
		}
		else
		{
			CU::Vector2<float> ratio = myBackground->GetSize() / anOldSize;
			myBackground->SetSize(aNewSize * ratio, { 0.f, 0.f });
		}
	}

	Widget* SpriteWidget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		return nullptr;
	}

}
