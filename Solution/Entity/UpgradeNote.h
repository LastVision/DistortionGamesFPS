#pragma once

struct UpgradeComponentData;

struct UpgradeNote
{
	UpgradeNote(const UpgradeComponentData& aData);

	void operator=(UpgradeNote&) = delete;

	const UpgradeComponentData& myData;
};

inline UpgradeNote::UpgradeNote(const UpgradeComponentData& aData)
	: myData(aData)
{
}