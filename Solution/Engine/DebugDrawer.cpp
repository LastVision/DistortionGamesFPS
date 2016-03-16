#include "stdafx.h"
#include "Cube3DRenderer.h"
#include "DebugDrawer.h"
#include "Engine.h"
#include "Line3DRenderer.h"
#include <sstream>
#ifdef RELEASE_BUILD
	#define WHITE_DEBUG CU::Vector4<float>(176.f/255.f, 176.f/255.f, 175.f/255.f, 1.f)
	#define BLACK_DEBUG CU::Vector4<float>(0.f, 0.f, 0.f, 1.f)
	#define RED_DEBUG CU::Vector4<float>(152.f/255.f, 104.f/255.f, 104.f/255.f, 1.f)
	#define GREEN_DEBUG CU::Vector4<float>(105.f / 255.f, 123.f/255.f, 103.f/255.f, 1.f)
	#define BLUE_DEBUG CU::Vector4<float>(0.f, 0.f, 1.f, 1.f)
	#define PINK_DEBUG CU::Vector4<float>(176.f/255.f, 112.f/255.f, 176.f/255.f, 1.f)
	#define YELLOW_DEBUG CU::Vector4<float>(1.f, 1.f, 0.f, 1.f)
	#define NOT_USED_DEBUG CU::Vector4<float>(-1.f, -1.f, -1.f, -1.f)
#else
	#define WHITE_DEBUG CU::Vector4<float>(1.f, 1.f, 1.f, 1.f)
	#define BLACK_DEBUG CU::Vector4<float>(0.f, 0.f, 0.f, 1.f)
	#define RED_DEBUG CU::Vector4<float>(1.f, 0.f, 0.f, 1.f)
	#define GREEN_DEBUG CU::Vector4<float>(0.f, 1.f, 0.f, 1.f)
	#define BLUE_DEBUG CU::Vector4<float>(0.f, 0.f, 1.f, 1.f)
	#define PINK_DEBUG CU::Vector4<float>(1.f, 0.f, 1.f, 1.f)
	#define YELLOW_DEBUG CU::Vector4<float>(1.f, 1.f, 0.f, 1.f)
	#define NOT_USED_DEBUG CU::Vector4<float>(-1.f, -1.f, -1.f, -1.f)
#endif



namespace Prism
{
	DebugDrawer* DebugDrawer::myInstance = nullptr;
	DebugDrawer* DebugDrawer::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new DebugDrawer();
		}

		return myInstance;
	}

	void DebugDrawer::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	//void DebugDrawer::RenderLine2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
	//	, const CU::Vector4<float>& aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	void DebugDrawer::RenderLine3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
		, eColorDebug aColor, eColorDebug aSecondColor)
	{
		my3DLines.Add(Line3D(aFirstPoint, aSecondPoint, GetColor(aColor), GetColor(aSecondColor)));
	}

	//void DebugDrawer::RenderArrow2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
	//	, const CU::Vector4<float>& aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	//void DebugDrawer::RenderArrow3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
	//	, eColorDebug aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	void DebugDrawer::RenderBox(const CU::Vector3<float>& aPosition, eColorDebug aColor, float aSize
		, bool aWireFrame)
	{
		myCube3DRenderer->AddCube(aPosition, aSize, GetColor(aColor), aWireFrame);
	}

	//void DebugDrawer::RenderSphere(const CU::Vector3<float>& aPosition, float aSize, eColorDebug aColor
	//	, bool aWireFrame)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	//void DebugDrawer::RenderText2D(const std::string& aText, const CU::Vector2<float>& aPosition
	//	, const CU::Vector4<float>& aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	//void DebugDrawer::RenderText3D(const std::string& aText, const CU::Vector3<float>& aPosition
	//	, eColorDebug aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	void DebugDrawer::RenderText(const char* aName, bool aValue)
	{
#ifndef RELEASE_BUILD
		myDebugTexts.Add(DebugText(aName, aValue ? "true" : "false"));
#endif
	}

	void DebugDrawer::RenderText(const char* aName, const CU::Vector2<float>& aValue)
	{
#ifndef RELEASE_BUILD

		std::stringstream ss;
		ss.precision(4);
		ss << "(" << aValue.x << ", " << aValue.y << ")";
		myDebugTexts.Add(DebugText(aName, ss.str()));
#endif
	}

	void DebugDrawer::RenderText(const char* aName, const CU::Vector3<float>& aValue)
	{
#ifndef RELEASE_BUILD

		std::stringstream ss;
		ss.precision(4);
		ss << "(" << aValue.x << ", " << aValue.y << ", " << aValue.z << ")";
		myDebugTexts.Add(DebugText(aName, ss.str()));
#endif
	}

	void DebugDrawer::RenderText(const char* aName, const CU::Vector4<float>& aValue)
	{
#ifndef RELEASE_BUILD

		std::stringstream ss;
		ss.precision(4);
		ss << "(" << aValue.x << ", " << aValue.y << ", " << aValue.z << ", " << aValue.w << ")";
		myDebugTexts.Add(DebugText(aName, ss.str()));
#endif
	}

	void DebugDrawer::RenderText(const char* aName, float aValue)
	{
#ifndef RELEASE_BUILD

		std::stringstream ss;
		ss.precision(4);
		ss << aValue;
		myDebugTexts.Add(DebugText(aName, ss.str()));
#endif
	}

	void DebugDrawer::RenderText(const char* aName, int aValue)
	{
#ifndef RELEASE_BUILD

		std::stringstream ss;
		ss.precision(4);
		ss << aValue;
		myDebugTexts.Add(DebugText(aName, ss.str()));
#endif
	}

	void DebugDrawer::RenderText(const char* aName, unsigned int aValue)
	{
#ifndef RELEASE_BUILD

		std::stringstream ss;
		ss.precision(4);
		ss << aValue;
		myDebugTexts.Add(DebugText(aName, ss.str()));
#endif
	}

	void DebugDrawer::RenderText(const char* aName, const std::string& aValue)
	{
#ifndef RELEASE_BUILD

		myDebugTexts.Add(DebugText(aName, aValue));
#endif
	}


	void DebugDrawer::RenderTextToScreen()
	{
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_DISABLED);
		CU::Vector2<float> window(Engine::GetInstance()->GetWindowSize());

		float rowHeight = 28.f;
		CU::Vector2<float> textPos(10.f, window.y - rowHeight * 2.f);
#ifdef RENDER_DEBUG_TEXT
		for (int i = 0; i < myDebugTexts.Size(); ++i)
		{
			Engine::GetInstance()->PrintText(myDebugTexts[i].myName + ": " + myDebugTexts[i].myValue
				, textPos, eTextType::DEBUG_TEXT, 1.f, { 1.f, 0.3f, 0.3f, 1.f });
			textPos.y -= rowHeight;
		}
#endif
		myDebugTexts.RemoveAll();

		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
	}

	void DebugDrawer::RenderLinesToScreen(const Camera& aCamera)
	{
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_DISABLED);

		myLine3DRenderer->Render(my3DLines, aCamera);
		myCube3DRenderer->Render(aCamera);
		
		my3DLines.RemoveAll();

		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
	}

	DebugDrawer::DebugDrawer()
		: my3DLines(1024)
		, myDebugTexts(64)
		, myLine3DRenderer(new Line3DRenderer())
		, myCube3DRenderer(new Cube3DRenderer())
	{
	}

	DebugDrawer::~DebugDrawer()
	{
		SAFE_DELETE(myLine3DRenderer);
		SAFE_DELETE(myCube3DRenderer);
	}

	CU::Vector4<float> DebugDrawer::GetColor(eColorDebug aColor) const
	{
		CU::Vector4<float> color;

		switch (aColor)
		{
			case eColorDebug::WHITE:
				color = WHITE_DEBUG;
				break;
			case eColorDebug::BLACK:
				color = BLACK_DEBUG;
				break;
			case eColorDebug::RED:
				color = RED_DEBUG;
				break;
			case eColorDebug::GREEN:
				color = GREEN_DEBUG;
				break;
			case eColorDebug::BLUE:
				color = BLUE_DEBUG;
				break;
			case eColorDebug::PINK:
				color = PINK_DEBUG;
				break;
			case eColorDebug::YELLOW:
				color = YELLOW_DEBUG;
				break;
			case eColorDebug::NOT_USED:
				color = NOT_USED_DEBUG;
				break;
			default:
				DL_ASSERT("Unknown color in DEBUGDRAWER");
				color = WHITE_DEBUG;
				break;
		}
		return color;
	}
}