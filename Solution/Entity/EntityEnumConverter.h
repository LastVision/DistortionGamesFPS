#pragma once

namespace EntityEnumConverter
{
	const eEntityType ConvertStringToEntityType(const std::string& aType);
	const ePropType ConvertStringToPropType(const std::string& aType);
	const eOwnerType ConvertStringToOwnerType(const std::string& aType);
}