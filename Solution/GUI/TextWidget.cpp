#include "stdafx.h"
#include "TextWidget.h"
#include <Engine.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <Text.h>
#include <TextMessage.h>
#include <FadeMessage.h>

namespace GUI
{
	TextWidget::TextWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: Widget()
		, myBackground(nullptr)
		, myText(nullptr)
		, myTextScale(1.f)
	{
		std::string backgroundPath;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textposition"), "x", myTextPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textposition"), "y", myTextPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "background"), "path", backgroundPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textscale"), "value", myTextScale);

		myBackground = Prism::ModelLoader::GetInstance()->LoadSprite(backgroundPath, mySize);

		PostMaster::GetInstance()->Subscribe(eMessageType::TEXT, this);
	}

	TextWidget::~TextWidget()
	{
		SAFE_DELETE(myBackground);
		PostMaster::GetInstance()->UnSubscribe(eMessageType::TEXT, this);
	}

	void TextWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsVisible == true && myText != nullptr)
		{
			myText->SetScale({ myTextScale, myTextScale });
			myText->SetPosition(myPosition + aParentPosition + myTextPosition);
			myBackground->Render(myPosition + aParentPosition);
			myText->Render();
		}
	}

	void TextWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewSize, anOldSize, aIsFullScreen);
		myBackground->SetSize(mySize, { 0.f, 0.f });
		myTextScale = (myTextScale / anOldSize.x) * aNewSize.x;

		if (myIsFullscreen == false)
		{
			myTextPosition = (myTextPosition / anOldSize.x) * aNewSize.x;
		}
		else
		{
			myTextPosition = (myTextPosition / anOldSize) * aNewSize;
		}
	}

	void TextWidget::ReceiveMessage(const TextMessage& aMessage)
	{
		if (aMessage.myMessageType == eMessageType::TEXT)
		{
			//PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
			myText = aMessage.myText;
			SetVisibility(aMessage.myVisibleFlag);
		}
	}
}