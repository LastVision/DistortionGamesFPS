#pragma once


#include "AK\SoundEngine\Common\AkTypes.h"
#include "AK\Tools\Common\AkPlatformFuncs.h"
#include "AK\SoundEngine\Common\AkSoundEngine.h"

#include "AK\SoundEngine\Common\AkMemoryMgr.h"		// Memory Manager
#include "AK\SoundEngine\Common\AkModule.h"		// Default memory and stream managers
#include "AK\SoundEngine\Common\IAkStreamMgr.h"		// Streaming Manager
#include "AK\SoundEngine\Common\AkSoundEngine.h"    // Sound engine
#include "AK\MusicEngine\Common\AkMusicEngine.h"	// Music Engine
#include "AK\SoundEngine\Common\AkStreamMgrModule.h"	// AkStreamMgrModule
#include "AK\Plugin\AllPluginsRegistrationHelpers.h"	// Plug-ins
#include "AK\Comm\AkCommunication.h"	// Plug-ins
#include "SoundEngine\Win32\AkFilePackageLowLevelIOBlocking.h"

namespace Prism
{
	namespace Audio
	{
		class WwiseManager
		{
		public:
			WwiseManager();
			~WwiseManager();
			bool Init(const char* aInitBank);
			void Update();
			bool LoadBank(const char* aBankPath);
			void UnLoadBank(const char* aBankPath);
			void PostEvent(const char* aEvent, int aObjectID);
			void SetRTPC(const char* aRTPC, int aValue, int aObjectID);
			void SetPosition(float aX, float aY, float aZ, int aObjectID);
			void SetListenerPosition(float aX, float aY, float aZ, float aForwardX, float aForwardY, float aForwardZ
				, float aUpX, float aUpY, float aUpZ);

			void RegisterObject(int anObjectID);

			int RegisterAndGetID();

			void UnRegister(int aObjectID);

			typedef void(*callback_function)(const char*);
			void SetErrorCallBack(callback_function aErrorCallback);
		private:
			void TermWwise();
			bool InitWwise(AkMemSettings &in_memSettings, AkStreamMgrSettings &in_stmSettings, AkDeviceSettings &in_deviceSettings, AkInitSettings &in_initSettings, AkPlatformInitSettings &in_platformInitSettings, AkMusicSettings &in_musicInit, AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount);
			void CallError(const char* aError);
			callback_function myErrorCallbck;

			int myCurrentID;
		};
	}
}
