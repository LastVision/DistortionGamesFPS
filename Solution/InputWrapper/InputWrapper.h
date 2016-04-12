#pragma once
#pragma comment(lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Vector.h>
#include <GrowingArray.h>

namespace std
{
	class thread;
}


namespace CU
{
	class TimerManager;
	class InputWrapper
	{
	public:
		UCHAR myKeyState[256];


		static void Create(HWND aHwnd, HINSTANCE aHInstance, DWORD aKeyCoopFlags, DWORD aMouseCoopFlags);
		static void Destroy();
		static InputWrapper* GetInstance();


		float GetMouseDX() const;
		float GetMouseDY() const;
		float GetMouseDZ() const; //scrollwheel

		const CU::Vector2<float>& GetMousePosition() const;

		bool MouseDown(int aButton) const; //specifik musknapp nere this frame
		bool MouseUp(int aButton) const;
		bool MouseIsPressed(int aButton) const; //musknapp nere 

		bool KeyDown(unsigned int aKey) const; //Returns true if specified key is down THIS FRAME
		bool KeyUp(unsigned int aKey) const;
		bool KeyIsPressed(unsigned int aKey) const; //Returns true if specified key is down 

		void FeedMouseRawInput(int aDeltaX, int aDeltaY);
		void PauseDeltaRecording();
		void ResumeDeltaRecording();
		void ToggleWindowActive();

		void TweakValue(float& aValue, float aRate, float aDeltaTime
			, unsigned int aIncreaseKey, unsigned int aDecreaseKey) const;
		void Update();
	private:
		CU::GrowingArray<CU::Vector2<int>> myBufferedMousePosition;
		CU::Vector2<int> myMouseDelta;
		InputWrapper();
		~InputWrapper();
		void Init(HWND aHwnd, HINSTANCE aHInstance, DWORD aKeyCoopFlags, DWORD aMouseCoopFlags);
		void CapturePreviousState();

		static InputWrapper* myInstance;

		std::thread* myUpdateThread;
		volatile bool myIsUpdating;

		LPDIRECTINPUT8 myDirectInput;
		LPDIRECTINPUTDEVICE8 myKeyboardDevice;
//		UCHAR myKeyState[256];
		UCHAR myPreviousKeyState[256];

		LPDIRECTINPUTDEVICE8 myMouseDevice;
		DIMOUSESTATE myMouseState;
		DIMOUSESTATE myPreviousMouseState;
		TimerManager* myTimerManager;
		CU::Vector2<float> myMousePos;

		HWND myWindowHandler;

		bool myIsRecordingDeltas;
		bool myWindowIsActive;
	};
}
