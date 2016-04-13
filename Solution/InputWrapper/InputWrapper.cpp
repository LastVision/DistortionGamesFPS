#include "InputWrapper.h"
#include <MemoryMacros.h>
#include <Defines.h>
#include "../Timer/TimerManager.h"

CU::InputWrapper* CU::InputWrapper::myInstance = nullptr;

void CU::InputWrapper::Create(HWND aHwnd, HINSTANCE aHInstance, DWORD aKeyCoopFlags, DWORD aMouseCoopFlags)
{
	assert(myInstance == nullptr && "InputWrapper already created!");
	if (myInstance == nullptr)
	{
		myInstance = new CU::InputWrapper();
		myInstance->Init(aHwnd, aHInstance, aKeyCoopFlags, aMouseCoopFlags);
	}
}

CU::InputWrapper* CU::InputWrapper::GetInstance()
{
	return myInstance;
}

void CU::InputWrapper::Destroy()
{
	SAFE_DELETE(myInstance);
}

CU::InputWrapper::InputWrapper()
	: myWindowIsActive(true)
{
	myBufferedMousePosition[0].Init(128);
	myBufferedMousePosition[1].Init(128);
}

void CU::InputWrapper::Init(HWND aHwnd, HINSTANCE aHInstance, DWORD aKeyCoopFlags, DWORD aMouseCoopFlags)
{

	DirectInput8Create(aHInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&myDirectInput), 0);

	myDirectInput->CreateDevice(GUID_SysKeyboard, &myKeyboardDevice, nullptr);
	myDirectInput->CreateDevice(GUID_SysMouse, &myMouseDevice, nullptr);

	myKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	myMouseDevice->SetDataFormat(&c_dfDIMouse);

	myKeyboardDevice->SetCooperativeLevel(aHwnd, aKeyCoopFlags);
	myMouseDevice->SetCooperativeLevel(aHwnd, aMouseCoopFlags);

	myKeyboardDevice->Acquire();
	myMouseDevice->Acquire();

	myWindowHandler = aHwnd;

	myIsRecordingDeltas = true;

	PhysicsUpdate();
	LogicUpdate();
}

CU::InputWrapper::~InputWrapper()
{
	myKeyboardDevice->Unacquire();
	myMouseDevice->Unacquire();
}

void CU::InputWrapper::LogicUpdate()
{
	myMouseDelta[eType::LOGIC].x = 0;
	myMouseDelta[eType::LOGIC].y = 0;
	for (int i = 0; i < myBufferedMousePosition[eType::LOGIC].Size(); ++i)
	{
		myMouseDelta[eType::LOGIC] += myBufferedMousePosition[eType::LOGIC][i];
	}
	myBufferedMousePosition[eType::LOGIC].RemoveAll();
	if (myWindowIsActive == true)
	{
		LogicCapturePreviousState();
		HRESULT hr = myKeyboardDevice->GetDeviceState(sizeof(myKeyState[eType::LOGIC]), reinterpret_cast<void**>(&myKeyState[eType::LOGIC]));

		if (FAILED(hr))
		{
			ZeroMemory(myKeyState[eType::LOGIC], sizeof(myKeyState[eType::LOGIC]));

			myKeyboardDevice->Acquire();
		}

		hr = myMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), reinterpret_cast<void**>(&myMouseState[eType::LOGIC]));
		if (FAILED(hr))
		{
			ZeroMemory(&myMouseState[eType::LOGIC], sizeof(myMouseState[eType::LOGIC]));

			myMouseDevice->Acquire();
		}

		tagPOINT cursorPoint;
		GetCursorPos(&cursorPoint);
		ScreenToClient(myWindowHandler, &cursorPoint);
		myMousePos[eType::LOGIC].x = static_cast<float>(cursorPoint.x);
		myMousePos[eType::LOGIC].y = static_cast<float>(cursorPoint.y);

		if (myIsRecordingDeltas == false)
		{
			myMouseState[eType::LOGIC].lX = 0;
			myMouseState[eType::LOGIC].lY = 0;
		}

		//myMousePos.x += myMouseState.lX;
		//myMousePos.y += myMouseState.lY;
		//myMousePos.z += myMouseState.lZ;
		//}
	}
}

void CU::InputWrapper::PhysicsUpdate()
{
	myMouseDelta[eType::PHYSICS].x = 0;
	myMouseDelta[eType::PHYSICS].y = 0;
	for (int i = 0; i < myBufferedMousePosition[eType::PHYSICS].Size(); ++i)
	{
		myMouseDelta[eType::PHYSICS] += myBufferedMousePosition[eType::PHYSICS][i];
	}
	myBufferedMousePosition[eType::PHYSICS].RemoveAll();
	if (myWindowIsActive == true)
	{
		PhysicsCapturePreviousState();
		HRESULT hr = myKeyboardDevice->GetDeviceState(sizeof(myKeyState[eType::PHYSICS]), reinterpret_cast<void**>(&myKeyState[eType::PHYSICS]));

		if (FAILED(hr))
		{
			ZeroMemory(myKeyState[eType::PHYSICS], sizeof(myKeyState[eType::PHYSICS]));

			myKeyboardDevice->Acquire();
		}
/*
		hr = myMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), reinterpret_cast<void**>(&myMouseState[eType::PHYSICS]));
		if (FAILED(hr))
		{
			ZeroMemory(&myMouseState[eType::PHYSICS], sizeof(myMouseState[eType::PHYSICS]));

			myMouseDevice->Acquire();
		}*/

		/*tagPOINT cursorPoint;
		GetCursorPos(&cursorPoint);
		ScreenToClient(myWindowHandler, &cursorPoint);
		myMousePos[eType::PHYSICS].x = static_cast<float>(cursorPoint.x);
		myMousePos[eType::PHYSICS].y = static_cast<float>(cursorPoint.y);

		if (myIsRecordingDeltas == false)
		{
			myMouseState[eType::PHYSICS].lX = 0;
			myMouseState[eType::PHYSICS].lY = 0;
		}*/

		//myMousePos.x += myMouseState.lX;
		//myMousePos.y += myMouseState.lY;
		//myMousePos.z += myMouseState.lZ;
		//}
	}
}
//
//void CU::InputWrapper::FeedMouseRawInput(int aDeltaX, int aDeltaY)
//{
//	myBufferedMousePosition.Add(CU::Vector2<int>(int(aDeltaX), int(aDeltaY)));
//}

void CU::InputWrapper::ToggleWindowActive()
{
	if (myInstance != nullptr)
	{
		myWindowIsActive = !myWindowIsActive;
	}
}

//void CU::InputWrapper::LogicTweakValue(float& aValue, float aRate, float aDeltaTime
//	, unsigned int aIncreaseKey, unsigned int aDecreaseKey) const
//{
//	if (myInstance != nullptr)
//	{
//		if (KeyIsPressed(aIncreaseKey))
//		{
//			aValue += aRate * aDeltaTime;
//		}
//		else if (KeyIsPressed(aDecreaseKey))
//		{
//			aValue -= aRate * aDeltaTime;
//		}
//	}
//}


//void CU::InputWrapper::PauseDeltaRecording()
//{
//	myIsRecordingDeltas = false;
//}
//
//
//void CU::InputWrapper::ResumeDeltaRecording()
//{
//	myIsRecordingDeltas = true;
//}

const CU::Vector2<float>& CU::InputWrapper::GetMousePosition(eType aType) const
{
	return myMousePos[aType];
}

void CU::InputWrapper::LogicCapturePreviousState()
{
	memcpy_s(myPreviousKeyState[eType::LOGIC], sizeof(myPreviousKeyState[eType::LOGIC]), myKeyState[eType::LOGIC], sizeof(myKeyState[eType::LOGIC]));
	memcpy_s(&myPreviousMouseState[eType::LOGIC], sizeof(myPreviousMouseState[eType::LOGIC]), &myMouseState[eType::LOGIC], sizeof(myMouseState[eType::LOGIC]));
}

void CU::InputWrapper::PhysicsCapturePreviousState()
{
	memcpy_s(myPreviousKeyState[eType::PHYSICS], sizeof(myPreviousKeyState[eType::PHYSICS]), myKeyState[eType::PHYSICS], sizeof(myKeyState[eType::PHYSICS]));
	//memcpy_s(&myPreviousMouseState[eType::PHYSICS], sizeof(myPreviousMouseState[eType::PHYSICS]), &myMouseState[eType::PHYSICS], sizeof(myMouseState[eType::PHYSICS]));
}

bool CU::InputWrapper::KeyIsPressed(unsigned int aKey, eType aType) const
{
	UCHAR key = static_cast<UCHAR>(aKey);
	return (myKeyState[aType][key] & 0x80) != 0;
}

bool CU::InputWrapper::KeyDown(unsigned int aKey, eType aType) const
{
	UCHAR key = static_cast<UCHAR>(aKey);
	return ((myKeyState[aType][key] & 0x80) != 0 && (myPreviousKeyState[aType][key] & 0x80) == 0);
}

bool CU::InputWrapper::KeyUp(unsigned int aKey, eType aType) const
{
	UCHAR key = static_cast<UCHAR>(aKey);
	return ((myKeyState[aType][key] & 0x80) == 0 && (myPreviousKeyState[aType][key] & 0x80) != 0);
}

bool CU::InputWrapper::MouseIsPressed(int aButton, eType aType) const
{
	return (myMouseState[aType].rgbButtons[aButton] & 0x80) != 0;
}

bool CU::InputWrapper::MouseDown(int aButton, eType aType) const
{
	return ((myMouseState[aType].rgbButtons[aButton] & 0x80) != 0 && (myPreviousMouseState[aType].rgbButtons[aButton] & 0x80) == 0);
}

bool CU::InputWrapper::MouseUp(int aButton, eType aType) const
{
	return ((myMouseState[aType].rgbButtons[aButton] & 0x80) == 0 && (myPreviousMouseState[aType].rgbButtons[aButton] & 0x80) != 0);
}

float CU::InputWrapper::GetMouseDX(eType aType) const
{
#ifdef USE_RAW_INPUT
	return myMouseDelta.x;
#else
	return static_cast<float>(myMouseState[aType].lX);
#endif
}

float CU::InputWrapper::GetMouseDY(eType aType) const
{
#ifdef USE_RAW_INPUT
	return myMouseDelta.y;
#else
	return static_cast<float>(myMouseState[aType].lY);
#endif
}

float CU::InputWrapper::GetMouseDZ(eType aType) const
{
	return static_cast<float>(myMouseState[aType].lZ);
}