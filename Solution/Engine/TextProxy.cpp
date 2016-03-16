#include "stdafx.h"

#include "Text.h"
#include "TextProxy.h"


namespace Prism
{
	TextProxy::TextProxy()
		: myText(nullptr)
		, myScale(1.f, 1.f)
		, myColor(1.f, 1.f, 1.f, 1.f)
	{
	}


	TextProxy::~TextProxy()
	{
		SAFE_DELETE(myText);
	}

	void TextProxy::SetPosition(const CU::Vector2<float>& aPosition)
	{
		myPosition.x = static_cast<float>(static_cast<int>(aPosition.x + 0.5f));
		myPosition.y = static_cast<float>(static_cast<int>(aPosition.y + 0.5f));
		myPosition.y -= Engine::GetInstance()->GetWindowSize().y;
	}

	void TextProxy::SetText(const std::string& aText)
	{
		if (IsLoaded() == true)
		{
			//NOT THREAD-SAFE
			myText->SetText(aText);



			//THREAD-SAFE
			//ModelLoader::GetInstance()->SetTextText(myText, aText);
		}
	}

	void TextProxy::SetColor(const CU::Vector4<float>& aColor)
	{
		myColor = aColor;
	}

	void TextProxy::Render()
	{
		if (IsLoaded() == true)
		{
			myText->Render(myPosition, myScale, myColor);
		}
	}

	void TextProxy::SetScale(const CU::Vector2<float>&)
	{
		//Scaling is not supported!
		myScale = CU::Vector2<float>(1.f, 1.f);

		float aspect = Engine::GetInstance()->GetWindowSize().x / Engine::GetInstance()->GetWindowSize().y;
		if (aspect < 1.5f)
		{
			myScale = CU::Vector2<float>(0.5f, 0.5f);
		}
	}

	float TextProxy::GetWidth() const
	{
		if (IsLoaded() == true)
		{
			return myText->GetWidth();
		}

		return 0.f;
	}

	bool TextProxy::IsLoaded() const
	{
		return myText != nullptr;
	}

	void TextProxy::SetText(Text* aText)
	{
		myText = aText;
	}
}