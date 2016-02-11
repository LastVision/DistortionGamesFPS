#pragma once
#include <string>

enum class eCooldownType
{
	TOTEM,
	UPGRADE,
	NONE
};

struct TooltipInfo
{
	TooltipInfo(std::string aHeadline, std::string aText, bool anIsLargeTooltip);

	const std::string myHeadline;
	const std::string myText;
	const bool myIsLargeTooltip;

	const int* myGunpowderCost;
	const int* myArftifactCost;
	const int* mySupplyCost;
	const float* myCooldown;
	eCooldownType myCooldownType;
	int myUpgradeLevel;

	void operator=(TooltipInfo&) = delete;
};

inline TooltipInfo::TooltipInfo(std::string aHeadline, std::string aText, bool anIsLargeTooltip)
	: myHeadline(aHeadline)
	, myText(aText)
	, myIsLargeTooltip(anIsLargeTooltip)
{
	myGunpowderCost = nullptr;
	myArftifactCost = nullptr;
	mySupplyCost = nullptr;
	myCooldown = nullptr;
	myCooldownType = eCooldownType::NONE;
	myUpgradeLevel = -1;
}