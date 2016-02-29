#include "stdafx.h"
#include <Game.h>
#include <ModelLoader.h>
#include <SetupInfo.h>
#include <string.h>
#include <istream>
#include <atlstr.h>
#include <TimerManager.h>
#include <CommonHelper.h>
#include <InputWrapper.h>
//#include <vld.h>

bool engineIsRunning = true;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool ReadSetup(Prism::SetupInfo& aSetup, const std::string& aFilePath);
void OnResize();

Game* globalGame = nullptr;
int globalClientWidth = 800;
int globalClientHeight = 600;
bool globalIsResizing = false;
bool globalPreviousFullscreenState = false;
bool globalIsActive = true;
Prism::SetupInfo globalSetup;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPTSTR, int aNumberCommands)
{
	ShowCursor(false);
	DL_Debug::Debug::Create();
	CU::TimerManager::Create();

	if (aNumberCommands > 2)
	{
		int commandCount = 0;
		LPWSTR* realCommands = CommandLineToArgvW(GetCommandLineW(), &commandCount);

		for (int i = 1; i < commandCount; ++i)
		{
			std::string command = CW2A(realCommands[i]);
			if (command == "-useEngineLog")
			{
				DL_Debug::Debug::GetInstance()->ActivateFilterLog(DL_Debug::eFilterLog::ENGINE);
			}
			else if (command == "-useGameLog")
			{
				DL_Debug::Debug::GetInstance()->ActivateFilterLog(DL_Debug::eFilterLog::GAME);
			}
			else if (command == "-useResourceLog")
			{
				DL_Debug::Debug::GetInstance()->ActivateFilterLog(DL_Debug::eFilterLog::RESOURCE);
			}
			else if (command == "-useDirectXLog")
			{
				DL_Debug::Debug::GetInstance()->ActivateFilterLog(DL_Debug::eFilterLog::DIRECTX);
			}
			else if (command == "-useFBXLog")
			{
				DL_Debug::Debug::GetInstance()->ActivateFilterLog(DL_Debug::eFilterLog::FBX);
			}
			else if (command == "-useFunctionTimerLog")
			{
				DL_Debug::Debug::GetInstance()->ActivateFilterLog(DL_Debug::eFilterLog::FUNCTION_TIMER);
			}
			else if (command == "-useEntityLog")
			{
				DL_Debug::Debug::GetInstance()->ActivateFilterLog(DL_Debug::eFilterLog::ENTITY);
			}
		}

		LocalFree(realCommands);
	}

	if (ReadSetup(globalSetup, CU::GetMyDocumentFolderPath() + "Machina\\Data\\Setting\\SET_config.bin") == false)
	{
		return 1;
	}

	HWND hwnd;

	if (Prism::Engine::Create(hwnd, WndProc, globalSetup) == false)
	{
		return 1;
	}

	globalGame = new Game();
	if (globalGame->Init(hwnd) == false)
	{
		return 1;
	}

	SetActiveWindow(hwnd);

	globalPreviousFullscreenState = Prism::Engine::GetInstance()->IsFullscreen();

#ifdef RELEASE_BUILD
	Prism::Engine::GetInstance()->SetFullscreen(true);
	globalPreviousFullscreenState = true;
#endif

	MSG msg;
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

			if (globalGame->Update() == false)
			{
				break;
			}


			Prism::Engine::GetInstance()->Render();
		}
	}
	engineIsRunning = false;
	globalGame->Destroy();
	delete globalGame;
	globalGame = nullptr;

	Prism::Engine::Destroy();
	MEMORY_TRACKER_DESTROY;
	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		CU::InputWrapper::GetInstance()->ToggleWindowActive();
		if (globalGame != nullptr)
		{
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				if (globalIsActive == true)
				{
					globalPreviousFullscreenState = Prism::Engine::GetInstance()->IsFullscreen();
					globalIsActive = false;
					globalGame->Pause();
				}
				
			}
			else
			{
				if (globalIsActive == false)
				{
					bool currFullscreen = Prism::Engine::GetInstance()->IsFullscreen();
					if (currFullscreen != globalPreviousFullscreenState)
					{
						Prism::Engine::GetInstance()->SetFullscreen(globalPreviousFullscreenState);

					}
#ifdef RELEASE_BUILD
					Prism::Engine::GetInstance()->SetFullscreen(true);
#endif

					globalIsActive = true;
					globalGame->UnPause();
				}
			}
		}
		break;
	case WM_SIZE:
	{
		globalClientWidth = LOWORD(lParam);
		globalClientHeight = HIWORD(lParam);

		if (globalClientWidth >= globalSetup.myScreenWidth) globalClientWidth = globalSetup.myScreenWidth;
		if (globalClientHeight >= globalSetup.myScreenHeight) globalClientHeight = globalSetup.myScreenHeight;

		if (LOWORD(wParam) == SIZE_MINIMIZED)
		{
			if (globalGame != nullptr)
			{
				globalGame->Pause();
			}
		}
		else if (LOWORD(wParam) == SIZE_MAXIMIZED)
		{
			OnResize();
		}
		else if (LOWORD(wParam) == SIZE_RESTORED)
		{
			if (globalIsResizing == false)
			{
				OnResize();
			}
		}
		break;
	}
	case WM_ENTERSIZEMOVE:
		if (globalGame != nullptr)
		{
			globalGame->Pause();
			globalIsResizing = true;
		}
		break;
	case WM_EXITSIZEMOVE:
		if (globalGame != nullptr)
		{
			globalIsResizing = false;
			OnResize();
			
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void OnResize()
{
	if (engineIsRunning == true)
	{
		Prism::ModelLoader::GetInstance()->Pause();
		Prism::Engine::GetInstance()->OnResize(globalClientWidth, globalClientHeight);

		if (globalGame != nullptr)
		{
			if (globalIsActive == true)
			{
				globalGame->UnPause();
			}
			globalGame->OnResize(globalClientWidth, globalClientHeight);
		}
		Prism::ModelLoader::GetInstance()->UnPause();
	}
}

bool ReadSetup(Prism::SetupInfo& aSetup, const std::string& aFilePath)
{
	int width = 800;
	int height = 600;
	int msaa = 4;
	int windowed = 1;

	std::ifstream file;
	file.open(aFilePath, std::ios::binary | std::ios::in);
	if (file.is_open() == true)
	{
		char buffer[4];

		file.read(buffer, 4);
		width = *(reinterpret_cast<int*>(buffer));

		file.read(buffer, 4);
		height = *(reinterpret_cast<int*>(buffer));

		file.read(buffer, 4);
		msaa = *(reinterpret_cast<int*>(buffer));

		file.read(buffer, 4);
		windowed = *(reinterpret_cast<int*>(buffer));
	}
	else 
	{
		MessageBox(NULL, "Could not find the config file. Please use the launcher instead.", "Error: Could not find config", MB_ICONERROR);
		return false;
	}
#ifndef RELEASE_BUILD
	windowed = true;
#endif

	aSetup.myScreenWidth = width;
	aSetup.myScreenHeight = height;
	aSetup.myMSAACount = msaa;

	if (windowed == 1)
		aSetup.myWindowed = true;
	else
		aSetup.myWindowed = false;
	return true;
}