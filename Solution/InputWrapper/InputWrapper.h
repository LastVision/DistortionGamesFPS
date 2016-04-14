#pragma once
#pragma comment(lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Vector.h>
#include <GrowingArray.h>

namespace CU
{
	class InputWrapper
	{
	public:
		enum eType
		{
			LOGIC = 0,
			PHYSICS = 1
		};

		static void Create(HWND aHwnd, HINSTANCE aHInstance, DWORD aKeyCoopFlags, DWORD aMouseCoopFlags);
		static void Destroy();
		static InputWrapper* GetInstance();


		float GetMouseDX(eType aType = eType::LOGIC) const;
		float GetMouseDY(eType aType = eType::LOGIC) const;
		float GetMouseDZ(eType aType = eType::LOGIC) const; //scrollwheel

		const CU::Vector2<float>& GetMousePosition(eType aType = eType::LOGIC) const;

		bool MouseDown(int aButton, eType aType = eType::LOGIC) const; //specifik musknapp nere this frame
		bool MouseUp(int aButton, eType aType = eType::LOGIC) const;
		bool MouseIsPressed(int aButton, eType aType = eType::LOGIC) const; //musknapp nere 

		bool KeyDown(unsigned int aKey, eType aType = eType::LOGIC) const; //Returns true if specified key is down THIS FRAME
		bool KeyUp(unsigned int aKey, eType aType = eType::LOGIC) const;
		bool KeyIsPressed(unsigned int aKey, eType aType = eType::LOGIC) const; //Returns true if specified key is down 

		bool AnyKeyDown(eType aType = eType::LOGIC) const;

		//void FeedMouseRawInput(int aDeltaX, int aDeltaY);
		//void PauseDeltaRecording();
		//void ResumeDeltaRecording();
		void ToggleWindowActive();

		void LogicTweakValue(float& aValue, float aRate, float aDeltaTime
			, unsigned int aIncreaseKey, unsigned int aDecreaseKey, eType aType = eType::LOGIC) const;

		void LogicUpdate();
		void PhysicsUpdate();

	private:
		CU::GrowingArray<CU::Vector2<int>> myBufferedMousePosition[2];
		CU::Vector2<int> myMouseDelta[2];

		InputWrapper();
		~InputWrapper();
		void Init(HWND aHwnd, HINSTANCE aHInstance, DWORD aKeyCoopFlags, DWORD aMouseCoopFlags);
		void LogicCapturePreviousState();
		void PhysicsCapturePreviousState();

		static InputWrapper* myInstance;


		LPDIRECTINPUT8 myDirectInput;
		LPDIRECTINPUTDEVICE8 myKeyboardDevice;
		UCHAR myKeyState[2][256];
		UCHAR myPreviousKeyState[2][256];

		LPDIRECTINPUTDEVICE8 myMouseDevice;
		DIMOUSESTATE myMouseState[2];
		DIMOUSESTATE myPreviousMouseState[2];

		CU::Vector2<float> myMousePos[2];

		HWND myWindowHandler;

		bool myIsRecordingDeltas;
		volatile bool myWindowIsActive;
	};
}
