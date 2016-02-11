#pragma once

#ifdef RELEASE_BUILD
#ifndef DLL_EXPORT
#define DL_ASSERT(string)
#define DL_ASSERT_EXP(expression, string)
#define DL_PRINT_VA(...)
#define DL_ASSERT_VA(...)
#define DL_PRINT(string)

#define DL_DEBUG( ... )

#define DL_MESSAGE_BOX(ERRORTEXT, TITLETEXT, TYPE)
#define DL_WRITELOG(log, ...)


#define ENGINE_LOG(...)
#define GAME_LOG(...)
#define RESOURCE_LOG(...)
#define DIRECTX_LOG(...)
#define FBX_LOG(...)
#define FUNCTION_TIMER_LOG(...)
#define ENTITY_LOG(...)
#define POWERUP_LOG(...)
#define COMPONENT_LOG(...)
#else
#define DL_ASSERT(string) DL_Debug::Debug::GetInstance()->AssertMessage(__FILE__,__LINE__,__FUNCTION__, string)
#define DL_ASSERT_EXP(expression, string)

#define DL_PRINT(string) DL_Debug::Debug::GetInstance()->PrintMessage(string)
#define DL_PRINT_VA(...)

#define DL_DEBUG( ... ) DL_Debug::Debug::GetInstance()->DebugMessage(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)

#define DL_MESSAGE_BOX(ERRORTEXT, TITLETEXT, TYPE)
#define DL_WRITELOG(log, ...)


#define ENGINE_LOG(...)
#define GAME_LOG(...)
#define RESOURCE_LOG(...)
#define DIRECTX_LOG(...)
#define FBX_LOG(...)
#define FUNCTION_TIMER_LOG(...)
#define ENTITY_LOG(...)
#define POWERUP_LOG(...)
#define COMPONENT_LOG(...)
#endif
#else
#define DL_ASSERT(string) DL_Debug::Debug::GetInstance()->AssertMessage(__FILE__,__LINE__,__FUNCTION__, string)
#define DL_ASSERT_VA(...) DL_Debug::Debug::GetInstance()->AssertMessageVA(__FILE__,__LINE__,__FUNCTION__, __VA_ARGS__)
#define DL_ASSERT_EXP(expression, string) DL_Debug::Debug::GetInstance()->AssertMessage(expression, __FILE__,__LINE__,__FUNCTION__, string)

#define DL_PRINT(string)  DL_Debug::Debug::GetInstance()->PrintMessage(string)
#define DL_PRINT_VA( ... ) DL_Debug::Debug::GetInstance()->PrintMessageVA(__VA_ARGS__)

#define DL_DEBUG( ... )  DL_Debug::Debug::GetInstance()->DebugMessage(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)

#define DL_MESSAGE_BOX(ERRORTEXT, TITLETEXT, TYPE) DL_Debug::Debug::GetInstance()->ShowMessageBox(NULL, ERRORTEXT, TITLETEXT, TYPE);
#define DL_WRITELOG(log, ...)  DL_Debug::Debug::GetInstance()->WriteLog(log,__VA_ARGS__);


#define ENGINE_LOG(...) DL_WRITELOG(DL_Debug::eFilterLog::ENGINE, __VA_ARGS__)
#define GAME_LOG(...) DL_WRITELOG(DL_Debug::eFilterLog::GAME, __VA_ARGS__)
#define RESOURCE_LOG(...) DL_WRITELOG(DL_Debug::eFilterLog::RESOURCE, __VA_ARGS__)
#define DIRECTX_LOG(...) DL_WRITELOG(DL_Debug::eFilterLog::DIRECTX, __VA_ARGS__)
#define FBX_LOG(...) DL_WRITELOG(DL_Debug::eFilterLog::FBX, __VA_ARGS__)
#define FUNCTION_TIMER_LOG(...) DL_WRITELOG(DL_Debug::eFilterLog::FUNCTION_TIMER, __VA_ARGS__)
#define ENTITY_LOG(...) DL_WRITELOG(DL_Debug::eFilterLog::ENTITY, __VA_ARGS__)
#define POWERUP_LOG(...) DL_WRITELOG(DL_Debug::eFilterLog::POWERUP_L, __VA_ARGS__)
#define COMPONENT_LOG(...) DL_WRITELOG(DL_Debug::eFilterLog::COMPONENT, __VA_ARGS__)
#endif


#include <string>
#include <fstream>
#include "DL_Assert.h"
#include "DL_StackWalker.h"
#include <unordered_map>

namespace DL_Debug
{
	enum class eFilterLog
	{
		ENGINE,
		GAME,
		RESOURCE,
		DIRECTX,
		FBX,
		FUNCTION_TIMER,
		ENTITY,
		POWERUP_L,
		COMPONENT,
		ALL
	};

	class Debug
	{
	public:

		static bool Create(std::string aFile = "DebugLogger.txt");
		static bool Destroy();
		static Debug* GetInstance();

		void WriteLog(const eFilterLog aFilter, const char* aFormattedString, ...);
		void PrintMessage(const char* aString);
		void PrintMessageVA(const char *aFormattedString, ...);
		void DebugMessage(const char *aFileName, int aLine, const char *aFunctionName, const char *aFormattedString, ...);
		void AssertMessage(bool aAssertExpression, const char *aFileName, int aLine, const char *aFunctionName, const char *aString);
		void AssertMessageVA(const char *aFileName, int aLine, const char *aFunctionName, const char *aFormattedString, ...);
		void AssertMessage(bool aAssertExpression, const char *aFileName, int aLine, const char *aFunctionName, const std::string& aString);
		void AssertMessage(const char *aFileName, int aLine, const char *aFunctionName, const std::string& aString);
		void ShowMessageBox(HWND aHwnd, LPCSTR aText, LPCSTR aTitle, UINT aType);

		void ActivateFilterLog(const eFilterLog aFilter);
		void DeactiveFilterLog(const eFilterLog aFilter);
	private:
		Debug();
		~Debug();
		static Debug* ourInstance;
		std::ofstream myDebugFile;
		std::unordered_map<eFilterLog, std::pair<bool, std::string>> myFilterLogStatus;
	};
}