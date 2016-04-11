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

	void Prism::TextProxy::SetOffset(const CU::Vector3<float>& aOffset)
	{
		my3DOrientation.SetPos(aOffset);
	}

	void Prism::TextProxy::Rotate3dText(float aRadians)
	{
		CU::Vector3<float> oldPos = my3DOrientation.GetPos();
		my3DOrientation.SetPos(CU::Vector3<float>());
		my3DOrientation = my3DOrientation * CU::Matrix44<float>::CreateRotateAroundY(aRadians);
		my3DOrientation.SetPos(oldPos);
	}

	void TextProxy::SetText(const std::string& aText)
	{
		myString = aText;
		if (IsLoaded() == true)
		{
			//NOT THREAD-SAFE
			//Prism::ModelLoader::GetInstance()->Pause();
			//myText->SetText(aText);
			//Prism::ModelLoader::GetInstance()->UnPause();



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
			if (myText->GetText() != myString)
			{
				Prism::ModelLoader::GetInstance()->Pause();
				myText->SetText(myString);
				Prism::ModelLoader::GetInstance()->UnPause();
			}

			myText->Render(myPosition, myScale, myColor);
		}
	}

	void TextProxy::Render(const Camera* aCamera)
	{
		if (IsLoaded() == true)
		{
			if (myText->GetText() != myString)
			{
				Prism::ModelLoader::GetInstance()->Pause();
				myText->SetText(myString);
				Prism::ModelLoader::GetInstance()->UnPause();
			}
			myText->Render(aCamera, my3DOrientation, myColor);
		}
	}

	void TextProxy::Render(const Camera* aCamera, float aScaleValue)
	{
		if (IsLoaded() == true)
		{
			if (myText->GetText() != myString)
			{
				Prism::ModelLoader::GetInstance()->Pause();
				myText->SetText(myString);
				Prism::ModelLoader::GetInstance()->UnPause();
			}
			myText->Render(aCamera, my3DOrientation, { aScaleValue, aScaleValue }, myColor);
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

	void TextProxy::SetOrientation(const CU::Matrix44<float>& aOrientation)
	{
		my3DOrientation = aOrientation;
	}

	CU::Vector3<float> TextProxy::Get3DPosition() const
	{
		return my3DOrientation.GetPos();
	}
}