#pragma once

#include "EngineEnums.h"
#include <Matrix.h>
#include <GrowingArray.h>
#include <Vector.h>
#include <string>
#include <thread>
#include <Windows.h>

struct ID3D11Device;
struct ID3D11DeviceChild;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

namespace Prism
{
	struct TextCommand
	{
		std::string myText;
		CU::Vector2<float> myPosition;
		CU::Vector4<float> myColor;
		float myScale;
	};

	enum class eTextType
	{
		DEBUG_TEXT,
		RELEASE_TEXT,
	};


	class Camera;
	class DirectX;
	class Effect;
	class FBXFactory;
	class FontProxy;
	class Model;
	class ModelProxy;
	class Sprite;
	class TextProxy;

	struct SetupInfo;

	class Engine
	{
	public:
		static bool Create(HWND& aHwnd, WNDPROC aWndProc, SetupInfo& aSetupInfo);
		static bool CreateOcculus(float aWidth, float aHeight, ID3D11Device* aDevice, ID3D11DeviceContext* aContext);
		static void Destroy();
		static Engine* GetInstance();
		void Update(float aDeltaTime);
		void Render();
		void OnResize(int aWidth, int aHeigth);
		bool IsFullscreen() const;
		void SetFullscreen(bool aFullscreen);

		ID3D11Device* GetDevice();
		void SetDevice(ID3D11Device* aDevice);
		ID3D11DeviceContext* GetContex();
		void SetContext(ID3D11DeviceContext* aContext);
		ID3D11DepthStencilView* GetDepthView();
		ID3D11RenderTargetView* GetBackbuffer();
		void SetDepthView(ID3D11DepthStencilView* aDepthView);
		void SetBackBuffer(ID3D11RenderTargetView* aBackBuffer);

		ID3D11ShaderResourceView* GetBackbufferView();
		ID3D11Texture2D* GetDepthBufferTexture();

		void SetDepthStencil(ID3D11DepthStencilView* aStencil);
		void RestoreDepthStencil();

		FontProxy* GetFont(eFont aFont);
		void SetDebugName(ID3D11DeviceChild* aChild, const std::string& aName);

		Model* DLLLoadModel(const std::string& aModelPath, Effect* aEffect);

		const CU::Vector2<float>& GetWindowSize() const;
		void SetWindowSize(const CU::Vector2<float>& aSize);
		const CU::Vector2<int>& GetWindowSizeInt() const;
		const CU::Matrix44<float>& GetOrthogonalMatrix() const;

		float PrintText(const std::string& aText, const CU::Vector2<float>& aPosition, eTextType aTextType, float aScale = 1.f, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f });
		void PrintText(float aNumber, const CU::Vector2<float>& aPosition, eTextType aTextType, float aScale = 1.f, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f });
		void PrintText(int aNumber, const CU::Vector2<float>& aPosition, eTextType aTextType, float aScale = 1.f, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f });

		void RenderText(TextProxy* aText);

		void RestoreViewPort();
		void SetBackBufferAsTarget();

		void SetDepthBufferState(eDepthStencil aState);
		eDepthStencil GetDepthBufferState() const;

		void SetRasterizeState(eRasterizer aState);
		eRasterizer GetRasterizerState() const;

		void SetShowDebugText(bool aShowDebug);
		void SetClearColor(const CU::Vector4<float>& aClearColor);

		bool UsePBLPixelShader();
		void TogglePBLPixelShader();

		void SetShouldRenderText(bool aStatus);

		void StartFade(float aDuration);

		bool myWireframeShouldShow;
		volatile bool myIsLoading;

	private:
		struct FadeData
		{
			Sprite* mySprite;
			float myCurrentTime;
			float myTotalTime;
			bool myIsFading;
		};

		Engine();
		~Engine();
		bool Init(HWND& aHwnd, WNDPROC aWndProc);
		bool InitOcculus(SetupInfo& aInfo, ID3D11Device* aDevice, ID3D11DeviceContext* aContext);
		bool WindowSetup(HWND& aHwnd, WNDPROC aWindowProc);
		
		bool myWireframeIsOn;
		bool myUsePBLPixelShader;
		bool myShouldRenderText;
		

		DirectX* myDirectX;
		SetupInfo* mySetupInfo;
		FBXFactory* myModelFactory;
		FontProxy* myDialogueFont;
		FontProxy* myConsoleFont;
		TextProxy* myText;
		TextProxy* myDebugText;
		FadeData myFadeData;

		CU::Vector4<float> myClearColor;
		CU::Vector2<int> myWindowSizeInt;
		CU::Vector2<float> myWindowSize;
		CU::Matrix44<float> myOrthogonalMatrix;


		std::thread* myModelLoaderThread;
		std::thread::id myModelLoaderThreadID;
		std::thread::id myMainThreadID;

		static Engine* myInstance;

		CU::GrowingArray<TextCommand> myTexts;
		CU::GrowingArray<TextCommand> myDebugTexts;
		bool myShowDebugText;
		CU::GrowingArray<TextProxy*> myTextsNew;
	};
}

inline const CU::Vector2<float>& Prism::Engine::GetWindowSize() const
{
	return myWindowSize;
}

inline void Prism::Engine::SetWindowSize(const CU::Vector2<float>& aSize)
{
	myWindowSize = aSize;
	myWindowSizeInt = CU::Vector2<int>(int(aSize.x), int(aSize.y));
}

inline const CU::Vector2<int>& Prism::Engine::GetWindowSizeInt() const
{
	return myWindowSizeInt;
}

inline const CU::Matrix44<float>& Prism::Engine::GetOrthogonalMatrix() const
{
	return myOrthogonalMatrix;
}

inline void Prism::Engine::SetClearColor(const CU::Vector4<float>& aClearColor)
{
	myClearColor = aClearColor;
}

inline void Prism::Engine::SetShowDebugText(bool aShowDebugText)
{
	myShowDebugText = aShowDebugText;
}

inline bool Prism::Engine::UsePBLPixelShader()
{
	return myUsePBLPixelShader;
}

inline void Prism::Engine::TogglePBLPixelShader()
{
	myUsePBLPixelShader = !myUsePBLPixelShader;
}