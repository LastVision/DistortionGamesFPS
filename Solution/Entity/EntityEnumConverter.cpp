#include "stdafx.h"

#include "EntityEnumConverter.h"

namespace EntityEnumConverter
{
	const eEntityType ConvertStringToEntityType(const std::string& aType)
	{
		if (aType == "prop")
		{
			return eEntityType::PROP;
		}
		else if (aType == "grenade")
		{
			return eEntityType::GRENADE;
		}
		else if (aType == "unit")
		{
			return eEntityType::UNIT;
		}
		else if (aType == "trigger")
		{
			return eEntityType::TRIGGER;
		}
		else if (aType == "player")
		{
			return eEntityType::PLAYER;
		}
		else if (aType == "explosion")
		{
			return eEntityType::EXPLOSION;
		}
		else if (aType == "door")
		{
			return eEntityType::PROP;
		}
		else if (aType == "bullet")
		{
			return eEntityType::BULLET;
		}
		else if (aType == "spawnpoint")
		{
			return eEntityType::SPAWNPOINT;
		}
		else if (aType == "visualexplosion")
		{
			return eEntityType::VISUAL_EXPLOSION;
		}

		DL_ASSERT("The " + aType + " entity type is not supported, please tell Daniel about it.");
		return eEntityType::_COUNT;
	}
	const ePropType ConvertStringToPropType(const std::string& aType)
	{
		aType;

		DL_ASSERT("Not in use. Remove assert when in use.");
		return ePropType::NOT_A_PROP;
	}

	const eOwnerType ConvertStringToOwnerType(const std::string& aType)
	{
		aType;

		DL_ASSERT("Not in use. Remove assert when in use.");
		return eOwnerType::NOT_USED;
	}

	const eWeaponType ConvertStringToWeaponType(const std::string& aType)
	{
		if (aType == "pistol")
		{
			return eWeaponType::PISTOL;
		}
		else if (aType == "shotgun")
		{
			return eWeaponType::SHOTGUN;
		}
		else if (aType == "grenade_launcher")
		{
			return eWeaponType::GRENADE_LAUNCHER;
		}

		DL_ASSERT("The " + aType + " weapon type is not supported.");
		return eWeaponType::PISTOL;
	}
}