#include "stdafx.h"

#include "AnimationSystem.h"
#include "DirectX.h"
#include "EffectContainer.h"
#include "Engine.h"
#include "FBXFactory.h"
#include "Font.h"
#include "Model.h"
#include "ModelLoader.h"
#include "ModelProxy.h"
#include "Sprite.h"
#include "Text.h"
#include "TextProxy.h"
#include <TimerManager.h>
#include "TextureContainer.h"
#include <Vector.h>
#include "VTuneApi.h"

#include <algorithm>

namespace Prism
{
	Engine* Engine::myInstance = nullptr;

	Engine::Engine()
		: myClearColor({ 0.8f, 0.125f, 0.8f, 1.0f })
		, myTexts(16)
		, myDebugTexts(16)
		, myTextsNew(256)
		, myShowDebugText(false)
		, myShouldRenderText(true)
	{
		myModelFactory = new FBXFactory();
		myWireframeIsOn = false;
		myWireframeShouldShow = false;

		myFadeData.myIsFading = false; 
		myFadeData.myCurrentTime = 0.f;
		myFadeData.myTotalTime = 0.f;
		myFadeData.mySprite = nullptr;
	}

	Engine::~Engine()
	{
		SAFE_DELETE(myFadeData.mySprite);
		SAFE_DELETE(myModelFactory);

		SAFE_DELETE(myText);
		SAFE_DELETE(myDebugText);

		TextureContainer::Destroy();
		EffectContainer::Destroy();
		AnimationSystem::Destroy();

		ModelLoader::GetInstance()->ClearLoadJobs();
		ModelLoader::GetInstance()->WaitUntilFinished();
		ModelLoader::GetInstance()->Shutdown();
		myModelLoaderThread->join();
		ModelLoader::Destroy();
		SAFE_DELETE(myModelLoaderThread);

		SAFE_DELETE(myDirectX);
	}

	bool Engine::Create(HWND& aHwnd, WNDPROC aWndProc, SetupInfo& aSetupInfo)
	{
		myInstance = new Engine();
		myInstance->mySetupInfo = &aSetupInfo;

		bool result = myInstance->Init(aHwnd, aWndProc);

		if (aSetupInfo.myWindowed == false)
		{
			myInstance->myDirectX->SetFullscreen(true);
		}

		myInstance->Render();

		return result;
	}

	void Engine::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	Engine* Engine::GetInstance()
	{
		return myInstance;
	}

	void Engine::Update(float aDeltaTime)
	{
		if (myFadeData.myIsFading == true)
		{
			myFadeData.myCurrentTime -= aDeltaTime;

			if (myFadeData.myCurrentTime <= 0.f)
			{
				myFadeData.myIsFading = false;
				myFadeData.myCurrentTime = 0.f;
			}
		}
	}

	void Engine::Render()
	{
		DEBUG_PRINT(GET_RUNTIME);
		DEBUG_PRINT(GetWindowSize());

		if (myShouldRenderText == true)
		{
			for (int i = 0; i < myTexts.Size(); ++i)
			{
				if (myIsLoading == false)
				{
					myText->SetText(myTexts[i].myText);
					myText->SetPosition(myTexts[i].myPosition);
					myText->SetScale({ myTexts[i].myScale, myTexts[i].myScale });
					myText->SetColor(myTexts[i].myColor);
					myText->Render();
				}
			}
		}
		myTexts.RemoveAll();

		for (int i = 0; i < myDebugTexts.Size(); ++i)
		{
			if (myIsLoading == false)
			{
				myDebugText->SetText(myDebugTexts[i].myText);
				myDebugText->SetPosition(myDebugTexts[i].myPosition);
				myDebugText->SetScale({ myDebugTexts[i].myScale, myDebugTexts[i].myScale });
				myDebugText->SetColor(myDebugTexts[i].myColor);
				myDebugText->Render();
			}
		}

		myDebugTexts.RemoveAll();
		for (int i = 0; i < myTextsNew.Size(); ++i)
		{
			myTextsNew[i]->Render();
		}
		myTextsNew.RemoveAll();

		if (myFadeData.myIsFading == true)
		{
			myFadeData.mySprite->Render({ 0.f, 0.f }, { 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f * myFadeData.myCurrentTime / myFadeData.myTotalTime });
		}

		myDirectX->Present(0, 0);

		if (myFadeData.myIsFading == false)
		{
			myFadeData.mySprite->CopyFromD3DTexture(myDirectX->GetBackbufferTexture());
		}

#ifdef RELEASE_BUILD
#ifndef DLL_EXPORT
		myClearColor = { 0.f, 0.f, 0.f, 1.f };
#endif
#endif

		float clearColor[4] = { myClearColor.x, myClearColor.y, myClearColor.z, 0.f };
		myDirectX->Clear(clearColor);

		VTUNE_EVENT_END();
	}

	void Engine::OnResize(int aWidth, int aHeigth)
	{
		SET_RUNTIME(false);
		myWindowSizeInt.x = aWidth;
		myWindowSizeInt.y = aHeigth;
		myWindowSize.x = static_cast<float>(myWindowSizeInt.x);
		myWindowSize.y = static_cast<float>(myWindowSizeInt.y);

		if (myDirectX != nullptr)
		{
			myDirectX->OnResize(aWidth, aHeigth);
		}

		myOrthogonalMatrix = CU::Matrix44<float>::CreateOrthogonalMatrixLH(myWindowSize.x
			, myWindowSize.y, 0.1f, 1000.f);

		if (myFadeData.mySprite != nullptr)
		{
			myFadeData.mySprite->ResizeTexture(myDirectX->GetBackbufferTexture());
			myFadeData.mySprite->SetSize(myWindowSize, { 0.f, 0.f });
		}
		RESET_RUNTIME;
	}

	bool Engine::IsFullscreen() const
	{
		return myDirectX->IsFullscreen();
	}

	void Engine::SetFullscreen(bool aFullscreen)
	{
		myDirectX->SetFullscreen(aFullscreen);
	}


	Model* Engine::DLLLoadModel(const std::string& aModelPath, Effect* aEffect)
	{
		Model* model = myModelFactory->LoadModel(aModelPath.c_str());
		model->SetEffect(aEffect);
		model->Init(1);

		return model;
	}

	ID3D11Device* Engine::GetDevice()
	{
#ifdef THREADED_LOADING
		DL_ASSERT_EXP(std::this_thread::get_id() == myModelLoaderThreadID ||
			ModelLoader::GetInstance()->IsLoading() == false, "Called GetDevice() from mainThread, while modelLoader is loading, not allowed!");
#endif
		return myDirectX->GetDevice();
	}

	ID3D11DeviceContext* Engine::GetContex()
	{
		return myDirectX->GetContex();
	}

	ID3D11DepthStencilView* Engine::GetDepthView()
	{
		return myDirectX->GetDepthStencil();
	}

	ID3D11RenderTargetView* Engine::GetBackbuffer()
	{
		return myDirectX->GetBackbuffer();
	}

	ID3D11ShaderResourceView* Engine::GetBackbufferView()
	{
		return myDirectX->GetBackbufferView();
	}

	ID3D11Texture2D* Engine::GetDepthBufferTexture()
	{
		return myDirectX->GetDepthbufferTexture();
	}

	FontProxy* Engine::GetFont(eFont aFont)
	{
		switch (aFont)
		{
		case Prism::eFont::DIALOGUE:
			return myDialogueFont;
			break;
		case Prism::eFont::CONSOLE:
			return myConsoleFont;
			break;
		default:
			DL_ASSERT("Tried to get invalid font");
			break;
		}

		return myDialogueFont;
	}

	void Engine::SetDebugName(ID3D11DeviceChild* aChild, const std::string& aName)
	{
		myDirectX->SetDebugName(aChild, aName);
	}

	bool Engine::Init(HWND& aHwnd, WNDPROC aWndProc)
	{
		myWindowSizeInt.x = mySetupInfo->myScreenWidth;
		myWindowSizeInt.y = mySetupInfo->myScreenHeight;
		myWindowSize.x = static_cast<float>(myWindowSizeInt.x);
		myWindowSize.y = static_cast<float>(myWindowSizeInt.y);

		if (WindowSetup(aHwnd, aWndProc) == false)
		{
			ENGINE_LOG("Failed to Create Window");
			return false;
		}

		myDirectX = new DirectX(aHwnd, *mySetupInfo);
		if (myDirectX == nullptr)
		{
			ENGINE_LOG("Failed to Setup DirectX");
			return false;
		}

		myModelLoaderThread = new std::thread(&ModelLoader::Run, ModelLoader::GetInstance());
		myModelLoaderThreadID = myModelLoaderThread->get_id();

		myFadeData.mySprite = new Sprite(myDirectX->GetBackbufferTexture(), { float(myWindowSize.x), float(myWindowSize.y) }, { 0.f, 0.f });

		ShowWindow(aHwnd, 10);
		UpdateWindow(aHwnd);

		myOrthogonalMatrix = CU::Matrix44<float>::CreateOrthogonalMatrixLH(static_cast<float>(myWindowSize.x)
			, static_cast<float>(myWindowSize.y), 0.1f, 1000.f);


		myDialogueFont = ModelLoader::GetInstance()->LoadFont("Data/Resource/Font/debugText.txt", { 256, 256 });
		myConsoleFont = ModelLoader::GetInstance()->LoadFont("Data/Resource/Font/consolab.ttf_sdf.txt", { 256, 256 });

		//myDialogueFont = new Font("Data/Resource/Font/debugText.txt", { 256, 256 });
		//myConsoleFont = new Font("Data/Resource/Font/consolab.ttf_sdf.txt", { 256, 256 });
		//myText = new Text(*myDialogueFont);

		myText = ModelLoader::GetInstance()->LoadText(myDialogueFont);
		myText->SetPosition({ 800.f, -300.f });
		myText->SetText("едц");
		myText->SetScale({ 1.f, 1.f });

		//myDebugText = new Text(*myDialogueFont);
		myDebugText = ModelLoader::GetInstance()->LoadText(myDialogueFont);
		myDebugText->SetPosition({ 800.f, -300.f });
		myDebugText->SetText("едц");
		myDebugText->SetScale({ 1.f, 1.f });
		myDebugText->SetColor({ 1.f, 0, 0, 0.8f });

		myUsePBLPixelShader = true;
		myIsLoading = false;

		myMainThreadID = std::this_thread::get_id();

		ENGINE_LOG("Engine Init Successful");
		return true;
	}

	float Engine::PrintText(const std::string& aText, const CU::Vector2<float>& aPosition, eTextType aTextType, float aScale, CU::Vector4<float> aColor)
	{
		TextCommand toAdd;
		toAdd.myText = aText;
		toAdd.myPosition = aPosition;
		toAdd.myScale = aScale;
		toAdd.myColor = aColor;
		if (aTextType == eTextType::RELEASE_TEXT)
		{
			myTexts.Add(toAdd);
		}
		else if (aTextType == eTextType::DEBUG_TEXT && myShowDebugText == true)
		{
			myDebugTexts.Add(toAdd);
		}



		return myText->GetWidth();
	}

	void Engine::PrintText(float aNumber, const CU::Vector2<float>& aPosition, eTextType aTextType, float aScale, CU::Vector4<float> aColor)
	{
		std::stringstream ss;
		ss.precision(3);
		ss << aNumber;
		PrintText(ss.str(), aPosition, aTextType, aScale, aColor);
	}

	void Engine::PrintText(int aNumber, const CU::Vector2<float>& aPosition, eTextType aTextType, float aScale, CU::Vector4<float> aColor)
	{
		std::stringstream ss;
		ss.precision(3);
		ss << aNumber;
		PrintText(ss.str(), aPosition, aTextType, aScale, aColor);
	}

	void Engine::RenderText(TextProxy* aText)
	{
		myTextsNew.Add(aText);
	}
	
	void Engine::RestoreViewPort()
	{
		myDirectX->RestoreViewPort();
	}

	void Engine::SetBackBufferAsTarget()
	{
		myDirectX->SetBackBufferAsTarget();
	}

	void Engine::SetDepthBufferState(eDepthStencil aState)
	{
		myDirectX->SetDepthBufferState(aState);
	}

	eDepthStencil Engine::GetDepthBufferState() const
	{
		return myDirectX->GetDepthBufferState();
	}

	void Engine::SetRasterizeState(eRasterizer aState)
	{
		myDirectX->SetRasterizeState(aState);
	}

	eRasterizer Engine::GetRasterizerState() const
	{
		return myDirectX->GetRasterizerState();
	}

	void Engine::SetShouldRenderText(bool aStatus)
	{
		myShouldRenderText = aStatus;
	}

	void Engine::StartFade(float aDuration)
	{
		myFadeData.myTotalTime = aDuration;
		myFadeData.myCurrentTime = aDuration;
		myFadeData.myIsFading = true;
	}

	bool Engine::WindowSetup(HWND& aHwnd, WNDPROC aWindowProc)
	{
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = aWindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = GetModuleHandle(NULL);
		wcex.hIcon = LoadIcon(GetModuleHandle(NULL), NULL);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = "DirectX Window";
		wcex.hIconSm = LoadIcon(wcex.hInstance, NULL);

		if (RegisterClassEx(&wcex) == FALSE)
		{
			ENGINE_LOG("Failed to register WindowClass");
			return FALSE;
		}

		RECT rc = { 0, 0, mySetupInfo->myScreenWidth, mySetupInfo->myScreenHeight };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		aHwnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			"DirectX Window",
			"DirectX Window",
			WS_OVERLAPPEDWINDOW,
			-2,
			-2,
			rc.right - rc.left,
			rc.bottom - rc.top,
			NULL, 
			NULL,
			GetModuleHandle(NULL),
			NULL);

		if (!aHwnd)
		{
			ENGINE_LOG("Failed to CreateWindow");
			return FALSE;
		}

		ENGINE_LOG("Window Setup Successful");
		return TRUE;
	}
}