#include "stdafx.h"
#include <ClientGame.h>
#include <ModelLoader.h>
#include <SetupInfo.h>
#include <string.h>
#include <istream>
#include <atlstr.h>
#include <TimerManager.h>
#include <CommonHelper.h>
#include <InputWrapper.h>
#include <GameConstants.h>
//#include <vld.h>

bool engineIsRunning = true;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool ReadSetup(Prism::SetupInfo& aSetup, const std::string& aFilePath);
void OnResize();

ClientGame* globalGame = nullptr;
int globalClientWidth = 800;
int globalClientHeight = 600;
bool globalIsResizing = false;
bool globalPreviousFullscreenState = false;
bool globalIsActive = true;
Prism::SetupInfo globalSetup;

void InitRawInput(const HWND&)
{
	/*RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = aHwnd;

	if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]) == false))
	{
		DL_ASSERT("RAwInputSetUp Failed!");
	}*/

	RAWINPUTDEVICE Rid[2];

	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
	Rid[0].hwndTarget = 0;

	//Rid[1].usUsagePage = 0x01;
	//Rid[1].usUsage = 0x06;
	//Rid[1].dwFlags = RIDEV_NOLEGACY;   // adds HID keyboard and also ignores legacy keyboard messages
	//Rid[1].hwndTarget = 0;

	if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) 
	{
		//registration failed. Call GetLastError for the cause of the error
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPTSTR, int aNumberCommands)
{
	ShowCursor(false);
	DL_Debug::Debug::Create();

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
			else if (command == "-generateLightData")
			{
				GC::GenerateLightData = true;
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

	globalGame = new ClientGame();
	if (globalGame->Init(hwnd) == false)
	{
		return 1;
	}

	SetActiveWindow(hwnd);

	globalPreviousFullscreenState = Prism::Engine::GetInstance()->IsFullscreen();


	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0)
	{
		DL_ASSERT("Errors...");
	}
	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL)
	{
		DL_ASSERT("Initialization failed...");
	}
	int nNoOfDevices = 0;
	if ((nNoOfDevices = GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST))) == ((UINT) - 1))
	{
		DL_ASSERT("Errors...");
	}
	RID_DEVICE_INFO rdi;
	rdi.cbSize = sizeof(RID_DEVICE_INFO);

	for (int i = 0; i < nNoOfDevices; i++)
	{
		UINT size = 256;
		TCHAR tBuffer[256] = { 0 };
		tBuffer[0] = '\0';
		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, tBuffer, &size) < 0)
		{
			// Error in reading device name
			DL_ASSERT("error reading device name");
		}

		//qDebug() << "Device Name: " << tBuffer;
		//_tprintf(L"Device Name: %s\n", tBuffer);

		UINT cbSize = rdi.cbSize;
		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &rdi, &cbSize) < 0)
		{
		}

		if (rdi.dwType == RIM_TYPEMOUSE)
		{
			rdi.mouse.dwId;
			rdi.mouse.dwNumberOfButtons;
			rdi.mouse.dwSampleRate;
		}
		if (rdi.dwType == RIM_TYPEKEYBOARD)
		{
			rdi.keyboard.dwKeyboardMode;
			rdi.keyboard.dwNumberOfFunctionKeys;
			rdi.keyboard.dwNumberOfIndicators;
			rdi.keyboard.dwNumberOfKeysTotal;
			rdi.keyboard.dwType;
			rdi.keyboard.dwSubType;
		}
		if (rdi.dwType == RIM_TYPEHID)
		{
			rdi.hid.dwVendorId;
			rdi.hid.dwProductId;
			rdi.hid.dwVersionNumber;
			rdi.hid.usUsage;
			rdi.hid.usUsagePage;
		}
	}

	free(pRawInputDeviceList);

#ifdef USE_RAW_INPUT
	InitRawInput(hwnd);
#endif

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

	LPBYTE lpb;
	UINT dwSize;
	RAWINPUT *raw;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_INPUT:
	{
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		lpb = new BYTE[dwSize];

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		{
			DL_ASSERT("GetRawInputData doesn't return correct size");
			//OutputDebugStr(TEXT("GetRawInputData doesn't return correct size\n"));
		}

		raw = (RAWINPUT*)lpb;
		int deltaX = raw->data.mouse.lLastX;
		int deltaY = raw->data.mouse.lLastY;
		if (CU::InputWrapper::GetInstance() != nullptr)
		{
			CU::InputWrapper::GetInstance()->FeedMouseRawInput(deltaX, deltaY);
		}
	}
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
#ifndef RELEASE_BUILD
					bool currFullscreen = Prism::Engine::GetInstance()->IsFullscreen();
					if (currFullscreen != globalPreviousFullscreenState)
					{
						Prism::Engine::GetInstance()->SetFullscreen(globalPreviousFullscreenState);

					}
#else
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
			CU::InputWrapper::GetInstance()->ToggleWindowActive();
		}
		else if (LOWORD(wParam) == SIZE_MAXIMIZED)
		{
			CU::InputWrapper::GetInstance()->ToggleWindowActive();
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
#ifdef RELEASE_BUILD
			Prism::Engine::GetInstance()->SetFullscreen(true);
			globalPreviousFullscreenState = true;
#endif
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
	int graphicsSetting = 0;

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

		//file.read(buffer, 4);
		//graphicsSetting = *(reinterpret_cast<int*>(buffer));
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

	//Low settings
	if (graphicsSetting == 0)
	{
		GC::EnableCheapAmbient = true;
		GC::EnableVSync = false;
	}
	else
	{
		GC::EnableCheapAmbient = false;
		GC::EnableVSync = true;
	}


	return true;
}