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
	
	template<typename T>
	void PrintEndl(const T& aSomethingToPrint, WORD aColour)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), aColour);
		std::cout << aSomethingToPrint << "\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); //Colour reset
	}
	template<typename T>
	void Print(const T& aSomethingToPrint, WORD aColour)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), aColour);
		std::cout << aSomethingToPrint;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); //Colour reset
	}
}
