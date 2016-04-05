#pragma once
#include <string>
#include <iostream>
#include <Windows.h>



enum eConsoleColor
{
	BLACK_TEXT,
	DARK_BLUE_TEXT,
	DARK_GREEN_TEXT,
	AQUA_TEXT,
	DARK_RED_TEXT,
	PURPLE_TEXT,
	GOLD_TEXT,
	SILVER_TEXT,
	GRAY_TEXT,
	BLUE_TEXT,
	LIGHT_GREEN_TEXT,
	LIGHT_BLUE_TEXT,
	RED_TEXT,
	LIGHT_PURPLE_TEXT,
	YELLOW_TEXT,
	WHITE_TEXT,

	DARK_BLUE_BACK = 16,
	DARK_GREEN_BACK = 32,
	AQUA_BACK = 48,
	DARK_RED_BACK = 64,
	PURPLE_BACK = 80,
	GOLD_BACK = 96,
	SILVER_BACK = 112,
	GRAY_BACK = 128,
	BLUE_BACK = 144,
	LIGHT_GREEN_BACK = 160,
	LIGHT_BLUE_BACK = 176,
	RED_BACK = 192,
	LIGHT_YELLOW_BACK = 208,
	YELLOW_BACK = 224,
	WHITE_BACK = 240,
};

namespace Utility
{
	inline void Printf(const char* aSomethingToPrint, WORD aColour)
	{
#ifdef _DEBUG
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), aColour);
		printf("\n%s", aSomethingToPrint);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); //Colour reset
#else
		aSomethingToPrint;
		aColour;
#endif
	}

	inline void Printf(const std::string& aSomethingToPrint, WORD aColour)
	{
#ifdef _DEBUG
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), aColour);
		printf("\n%s", aSomethingToPrint.c_str());
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); //Colour reset
#else
		aSomethingToPrint;
		aColour;
#endif
	}

	inline void Printf(int aSomethingToPrint, WORD aColour)
	{
#ifdef _DEBUG
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), aColour);
		printf("\n%i", aSomethingToPrint);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); //Colour reset
#else
		aSomethingToPrint;
		aColour;
#endif
	}

	inline HWND GetConsoleHwnd(void)
	{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
		HWND hwndFound;         // This is what is returned to the caller.
		char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
		// WindowTitle.
		char pszOldWindowTitle[MY_BUFSIZE]; // Contains original
		// WindowTitle.

		// Fetch current window title.

		GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

		// Format a "unique" NewWindowTitle.

		wsprintf(pszNewWindowTitle, "%d/%d",
			GetTickCount(),
			GetCurrentProcessId());

		// Change current window title.

		SetConsoleTitle(pszNewWindowTitle);

		// Ensure window title has been updated.

		Sleep(40);

		// Look for NewWindowTitle.

		hwndFound = FindWindow(NULL, pszNewWindowTitle);

		// Restore original window title.

		SetConsoleTitle(pszOldWindowTitle);

		return(hwndFound);
	}

}
