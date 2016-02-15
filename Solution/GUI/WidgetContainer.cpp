#include "stdafx.h"
#include <Engine.h>
#include "WidgetContainer.h"

namespace GUI
{
	WidgetContainer::WidgetContainer(Prism::SpriteProxy* aBackgroundSprite,
			Prism::SpriteProxy* aVignette, const CU::Vector2<float>& aSize
			, bool aIsFullscreen, bool aIsScrolling)
		: Widget()
		, myBackground(aBackgroundSprite)
		, myVignette(aVignette)
		, myWidgets(8)
		, myIsScrolling(aIsScrolling)
	{
		myIsFullscreen = aIsFullscreen;
		mySize = aSize;
	}

	WidgetContainer::~WidgetContainer()
	{
		myWidgets.DeleteAll();
		SAFE_DELETE(myBackground);
		SAFE_DELETE(myVignette);
	}

	void WidgetContainer::AddWidget(Widget* aWidget)
	{
		myWidgets.Add(aWidget);
	}

	static float totalTime = 0;
	void WidgetContainer::Update(float aDelta)
	{
		if (myIsScrolling == true)
		{
			totalTime += aDelta * 0.2f;
			
		}
		for (int i = 0; i < myWidgets.Size(); i++)
		{
			myWidgets[i]->Update(aDelta);
		}
	}

	void WidgetContainer::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsScrolling == true)
		{
			myScrollOffset.x = sinf(totalTime) * 100.f;
		}
		if (myIsVisible == true)
		{
			if (myBackground != nullptr)
			{
				myBackground->Render(myPosition + myScrollStaticOffset + myScrollOffset);
			}
			if (myVignette != nullptr)
			{
				myVignette->Render(myPosition);
			}

			for (int i = 0; i < myWidgets.Size(); i++)
			{
				myWidgets[i]->Render(myPosition + aParentPosition);
			}
		}
	}

	Widget* WidgetContainer::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (IsInside(aPosition) == true)
		{
			for (int i = 0; i < myWidgets.Size(); i++)
			{
				if (myWidgets[i]->IsVisible() == true && myWidgets[i]->IsInside(aPosition - myPosition) == true)
				{
					Widget* childWidget = myWidgets[i]->MouseIsOver(aPosition - myPosition);
					if (childWidget != nullptr)
					{
						return childWidget;
					}
				}
			}

			return this;
		}
		return nullptr;
	}

	void WidgetContainer::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);

		for (int i = 0; i < myWidgets.Size(); i++)
		{
			myWidgets[i]->OnResize(aNewSize, anOldSize);
		}

		if (myBackground != nullptr)
		{
			if (myIsScrolling == true)
			{
				myScrollStaticOffset = aNewSize * 0.5f;
				myBackground->SetSize(CU::Vector2<float>(aNewSize.y * 2.f, aNewSize.y), CU::Vector2<float>(aNewSize.y, aNewSize.y * 0.5f));
			}
			else if (myIsFullscreen == false)
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
		if (myVignette != nullptr)
		{
			//CU::Vector2<float> ratio = myVignette->GetSize() / anOldSize.x;
			myVignette->SetSize(aNewSize, { 0.f, 0.f });
		}
	}
}