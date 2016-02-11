#pragma once
#include <Vector.h>
#include "TooltipInfo.h"

namespace GUI
{
	class Widget
	{
	public:
		Widget();
		virtual ~Widget();
		virtual void Destroy();

		virtual void Update(float aDelta);
		virtual void Render(const CU::Vector2<float>& aParentPosition);
		virtual void OnMouseDown(const CU::Vector2<float>& aPosition);
		virtual void OnRightMouseDown(const CU::Vector2<float>& aPosition);
		virtual void OnMousePressed(const CU::Vector2<float>& aPosition);
		virtual void OnRightMousePressed(const CU::Vector2<float>& aPosition);
		virtual void OnMouseUp();
		virtual void OnMouseEnter();
		virtual void OnMouseExit();
		virtual void OnMouseMoved(const CU::Vector2<float>& aPosition);

		virtual Widget*	MouseIsOver(const CU::Vector2<float>& aPosition);

		virtual bool IsInside(const CU::Vector2<float>& aPosition) const;

		virtual void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen);

		virtual inline void SetPosition(const CU::Vector2<float>& aPosition, bool aIsHotspot = true);
		virtual inline void SetVisibility(bool aVisibility);
		inline const CU::Vector2<float>& GetSize() const;
		inline const CU::Vector2<float>& GetPosition() const;
		inline bool IsVisible() const;

		const TooltipInfo* GetTooltipInfo() const;

		virtual inline void SetValue(const float& aValue);
		virtual inline void SetValue(const float& aFirstValue, const float& aSecondValue);

		inline Widget* GetParent() const;
		inline void SetParent(Widget* aParent);

		inline bool IsFullscreen() const;
		inline void SetIsFullscreen(bool aIsFullscreen);

		inline bool IsClickable() const;
		inline void SetIsClickable(bool aIsClickable);
	protected:
		CU::Vector2<float> myPosition;
		CU::Vector2<float> mySize;

		TooltipInfo* myTooltipInfo;

		bool myIsVisible;
		bool myIsClickable;
		bool myIsFullscreen;
		Widget* myParent;
	};

	inline void Widget::SetPosition(const CU::Vector2<float>& aPosition, bool)
	{
		myPosition = aPosition;
	}

	inline void Widget::SetVisibility(bool aVisibility)
	{
		myIsVisible = aVisibility;
	}

	inline const CU::Vector2<float>& Widget::GetSize() const
	{
		return mySize;
	}

	inline const CU::Vector2<float>& Widget::GetPosition() const
	{
		return myPosition;
	}

	inline bool Widget::IsVisible() const
	{
		return myIsVisible;
	}

	inline const TooltipInfo* Widget::GetTooltipInfo() const
	{
		return myTooltipInfo;
	}
	
	inline void Widget::SetValue(const float&)
	{
	}

	inline void Widget::SetValue(const float&, const float&)
	{
	}

	inline Widget* Widget::GetParent() const
	{
		return myParent;
	}

	inline void Widget::SetParent(Widget* aParent)
	{
		myParent = aParent;
	}

	inline bool Widget::IsFullscreen() const
	{
		return myIsFullscreen;
	}

	inline void Widget::SetIsFullscreen(bool aIsFullscreen)
	{
		myIsFullscreen = aIsFullscreen;
	}

	inline bool Widget::IsClickable() const
	{
		return myIsClickable;
	}

	inline void Widget::SetIsClickable(bool aIsClickable)
	{
		myIsClickable = aIsClickable;
	}
}