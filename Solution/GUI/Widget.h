#pragma once
#include <Vector.h>

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

		virtual void OnLeftMouseDown(const CU::Vector2<float>& aPosition);
		virtual void OnLeftMousePressed(const CU::Vector2<float>& aPosition);
		virtual void OnRightMouseDown(const CU::Vector2<float>& aPosition);
		virtual void OnRightMousePressed(const CU::Vector2<float>& aPosition);
		virtual void OnLeftMouseUp();
		virtual void OnRightMouseUp();
		virtual void OnMouseEnter();
		virtual void OnMouseExit();
		virtual void OnMouseMoved(const CU::Vector2<float>& aPosition);

		virtual Widget*	MouseIsOver(const CU::Vector2<float>& aPosition);

		virtual bool IsInside(const CU::Vector2<float>& aPosition) const;

		virtual void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize);

		virtual inline void SetPosition(const CU::Vector2<float>& aPosition, bool aIsHotspot = true);
		virtual inline void SetVisibility(bool aVisibility);
		inline const CU::Vector2<float>& GetSize() const;
		inline const CU::Vector2<float>& GetPosition() const;
		inline bool IsVisible() const;

	
		inline bool IsFullscreen() const;
		inline void SetIsFullscreen(bool aIsFullscreen);

		inline bool IsClickable() const;
		inline void SetIsClickable(bool aIsClickable);
	protected:
		CU::Vector2<float> myPosition;
		CU::Vector2<float> mySize;

		bool myIsVisible;
		bool myIsClickable;
		bool myIsFullscreen;
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