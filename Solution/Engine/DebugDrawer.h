#pragma once

#include "Line3D.h"
#include <GrowingArray.h>

#define DEBUG_PRINT(aVariable) Prism::DebugDrawer::GetInstance()->RenderText(#aVariable, aVariable)
#define DEBUG_PRINT_LUA(aVariable) Prism::DebugDrawer::GetInstance()->RenderText("LUA", aVariable)

enum class eColorDebug
{
	WHITE,
	BLACK,
	RED,
	GREEN,
	BLUE,
	PINK,
	YELLOW,
	NOT_USED
};

namespace Prism
{
	class Camera;
	class Line3DRenderer;
	class Cube3DRenderer;

	class DebugDrawer
	{
	public:
		static DebugDrawer* GetInstance();
		static void Destroy();

		//void RenderLine2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
		//	, const CU::Vector4<float>& aColor = PINK_DEBUG);
		void RenderLine3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
			, eColorDebug aColor = eColorDebug::PINK, eColorDebug aSecondColor = eColorDebug::NOT_USED);
		//void RenderArrow2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
		//	, const CU::Vector4<float>& aColor = PINK_DEBUG);
		//void RenderArrow3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
		//	, eColorDebug aColor = eColorDebug::PINK);
		void RenderBox(const CU::Vector3<float>& aPosition, eColorDebug aColor = eColorDebug::PINK, float aSize = 0.2f
			, bool aWireFrame = false);
		//void RenderSphere(const CU::Vector3<float>& aPosition, float aSize, eColorDebug aColor = eColorDebug::PINK
		//	, bool aWireFrame = false);
		//void RenderText2D(const std::string& aText, const CU::Vector2<float>& aPosition
		//	, const CU::Vector4<float>& aColor = PINK_DEBUG);
		//void RenderText3D(const std::string& aText, const CU::Vector3<float>& aPosition
		//	, eColorDebug aColor = eColorDebug::PINK);

		void RenderText(const char* aName, bool aValue);
		void RenderText(const char* aName, const CU::Vector2<float>& aValue);
		void RenderText(const char* aName, const CU::Vector3<float>& aValue);
		void RenderText(const char* aName, const CU::Vector4<float>& aValue);
		void RenderText(const char* aName, float aValue);
		void RenderText(const char* aName, int aValue);
		void RenderText(const char* aName, unsigned int aValue);
		void RenderText(const char* aName, const std::string& aValue);

		void RenderTextToScreen();
		void RenderLinesToScreen(const Camera& aCamera);

	private:

		struct DebugText
		{
			DebugText(){}
			DebugText(const char* aName, const std::string& aValue) : myName(aName), myValue(aValue){}
			std::string myName;
			std::string myValue;
		};

		static DebugDrawer* myInstance;
		DebugDrawer();
		~DebugDrawer();

		Line3DRenderer* myLine3DRenderer;
		Cube3DRenderer* myCube3DRenderer;

		CU::GrowingArray<Line3D> my3DLines;
		CU::GrowingArray<DebugText> myDebugTexts;

		CU::Vector4<float> GetColor(eColorDebug aColor) const;

	};

#pragma warning(disable: 4505)

	static void RenderLine3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
		, eColorDebug aColor = eColorDebug::PINK, eColorDebug aSecondColor = eColorDebug::NOT_USED)
	{
		DebugDrawer::GetInstance()->RenderLine3D(aFirstPoint, aSecondPoint, aColor, aSecondColor);
	}

	static void RenderBox(const CU::Vector3<float>& aPosition, eColorDebug aColor = eColorDebug::PINK, float aSize = 0.2f
		, bool aWireFrame = false)
	{
		DebugDrawer::GetInstance()->RenderBox(aPosition, aColor, aSize, aWireFrame);
	}
}