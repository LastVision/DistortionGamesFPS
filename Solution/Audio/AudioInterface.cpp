#include "stdafx.h"


#include "AudioInterface.h"
#include "WwiseManager.h"

#include <MemoryMacros.h> //Needs to be after WwiseManager to not Override their new


namespace Prism
{
	namespace Audio
	{
		AudioInterface* AudioInterface::myInstance = nullptr;
		AudioInterface::AudioInterface()
			:myWwiseManager(nullptr)
			, mySFXVolume(80)
			, myMusicVolume(70)
			, myVoiceVolume(80)
		{	
		}

		AudioInterface::~AudioInterface()
		{
			delete myWwiseManager;
		}

		bool AudioInterface::Init(const char* aInitBank)
		{
			myWwiseManager = new WwiseManager();
			return myWwiseManager->Init(aInitBank);
		}

		void AudioInterface::Update()
		{
			if (myWwiseManager)
			{
				myWwiseManager->Update();
			}

		}

		void AudioInterface::SetRTPC(const char* aRTPC, int aValue, int anObjectID)
		{
			if (myWwiseManager)
			{
				myWwiseManager->SetRTPC(aRTPC, aValue, anObjectID);
			}
		}

		void AudioInterface::SetPosition(float aX, float aY, float aZ, int aObjectID)
		{
			if (myWwiseManager)
			{
				myWwiseManager->SetPosition(aX, aY, aZ, aObjectID);
			}
		}

		void AudioInterface::SetListenerPosition(float aX, float aY, float aZ, float aForwardX, float aForwardY, float aForwardZ
			, float aUpX, float aUpY, float aUpZ)
		{
			if (myWwiseManager)
			{
				myWwiseManager->SetListenerPosition(aX, aY, aZ, aForwardX, aForwardY, aForwardZ, aUpX, aUpY, aUpZ);
			}
		}

		bool AudioInterface::LoadBank(const char* aBankPath)
		{
			if (myWwiseManager)
			{
				return myWwiseManager->LoadBank(aBankPath);
			}
			return false;
		}

		void AudioInterface::UnLoadBank(const char* aBankPath)
		{
			if (myWwiseManager)
			{
				return myWwiseManager->UnLoadBank(aBankPath);
			}
		}

		void AudioInterface::PostEvent(const char* aEvent, int anObjectID)
		{
			if (myWwiseManager)
			{
				std::string eventName(aEvent);
				if (eventName == "IncreaseVolume")
				{
					if (mySFXVolume >= 100)
					{
						mySFXVolume = 100;
						return;
					}
					mySFXVolume += 5;
				}
				else if (eventName == "LowerVolume")
				{
					if (mySFXVolume <= 0)
					{
						mySFXVolume = 0;
						return;
					}
					mySFXVolume -= 5;
				}
				else if (eventName == "IncreaseMusic")
				{
					if (myMusicVolume >= 100)
					{
						myMusicVolume = 100;
						return;
					}
					myMusicVolume += 5;
				}
				else if (eventName == "LowerMusic")
				{
					if (myMusicVolume <= 0)
					{
						myMusicVolume = 0;
						return;
					}
					myMusicVolume -= 5;
				}
				else if (eventName == "IncreaseVoice")
				{
					if (myVoiceVolume >= 100)
					{
						myVoiceVolume = 100;
						return;
					}
					myVoiceVolume += 5;
				}
				else if (eventName == "LowerVoice")
				{
					if (myVoiceVolume <= 0)
					{
						myVoiceVolume = 0;
						return;
					}
					myVoiceVolume -= 5;
				}

				return myWwiseManager->PostEvent(aEvent, anObjectID);
			}
		}

		void AudioInterface::SetErrorCallBack(callback_function aErrorCallback)
		{
			if (myWwiseManager)
			{
				return myWwiseManager->SetErrorCallBack(aErrorCallback);
			}
		}

		void AudioInterface::RegisterObject(int anObjectID)
		{
			if (myWwiseManager)
			{
				return myWwiseManager->RegisterObject(anObjectID);
			}
		}

		void AudioInterface::UnRegisterObject(int aObjectID)
		{
			if (myWwiseManager)
			{
				return myWwiseManager->UnRegister(aObjectID);
			}
		}

		int AudioInterface::GetUniqueID()
		{
			if (myWwiseManager)
			{
				return myWwiseManager->RegisterAndGetID();
			}
			return -1;
		}
	}
}