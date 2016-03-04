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
		else if (aType == "projectile")
		{
			return eEntityType::PROJECTILE;
		}
		else if (aType == "unit")
		{
			return eEntityType::UNIT;
		}
		else if (aType == "trigger")
		{
			return eEntityType::TRIGGER;
		}

		DL_ASSERT("The " + aType + " entity type is not supported, please tell Daniel about it.");
		return eEntityType::_COUNT;
	}
	const ePropType ConvertStringToPropType(const std::string& aType)
	{
		if (aType == "pinetree")
		{
			return ePropType::PINE_TREE_A;
		}
		else if (aType == "birchtree")
		{
			return ePropType::BIRCH_TREE_A;
		}
		else if (aType == "house_large")
		{
			return ePropType::HOUSE_LARGE;
		}
		else if (aType == "house_medium")
		{
			return ePropType::HOUSE_MEDIUM;
		}
		else if (aType == "house_small")
		{
			return ePropType::HOUSE_SMALL;
		}
		else if (aType == "sm_birch_tree_bare_a")
		{
			return ePropType::BIRCH_TREE_A;
		}
		else if (aType == "sm_pine_tree_bare_a")
		{
			return ePropType::PINE_TREE_A;
		}
		else if (aType == "sm_pine_tree_bare_b")
		{
			return ePropType::PINE_TREE_B;
		}
		else if (aType == "sm_pine_tree_bare_c")
		{
			return ePropType::PINE_TREE_C;
		}
		else if (aType == "sm_pine_tree_bare_d")
		{
			return ePropType::PINE_TREE_D;
		}
		else if (aType == "sm_house_grp")
		{
			return ePropType::HOUSE_GROUP;
		}
		else if (aType == "sm_house_large")
		{
			return ePropType::HOUSE_LARGE;
		}
		else if (aType == "sm_house_medium")
		{
			return ePropType::HOUSE_MEDIUM;
		}
		else if (aType == "sm_house_small")
		{
			return ePropType::HOUSE_SMALL;
		}
		else if (aType == "resource_pole")
		{
			return ePropType::RESOURCE_POLE;
		}
		else if (aType == "victory_pole")
		{
			return ePropType::VICTORY_POLE;
		}
		else if (aType == "sm_bush_a")
		{
			return ePropType::BUSH_A;
		}
		else if (aType == "sm_bush_b")
		{
			return ePropType::BUSH_B;
		}
		else if (aType == "sm_bush_c")
		{
			return ePropType::BUSH_C;
		}
		else if (aType == "sm_rock_large")
		{
			return ePropType::ROCK_LARGE;
		}
		else if (aType == "sm_rock_medium")
		{
			return ePropType::ROCK_MEDIUM;
		}
		else if (aType == "sm_rock_small")
		{
			return ePropType::ROCK_SMALL;
		}
		else if (aType == "sm_rocks_grp_a")
		{
			return ePropType::ROCKS_GROUP_A;
		}
		else if (aType == "sm_rocks_grp_b")
		{
			return ePropType::ROCKS_GROUP_B;
		}
		else if (aType == "sm_bones_medium")
		{
			return ePropType::BONES_MEDIUM;
		}
		else if (aType == "sm_bones_small")
		{
			return ePropType::BONES_SMALL;
		}
		else if (aType == "sm_brokenship_a")
		{
			return ePropType::BROKENSHIP_A;
		}
		else if (aType == "sm_brokenship_b")
		{
			return ePropType::BROKENSHIP_B;
		}
		else if (aType == "sm_brokensubmarine_a")
		{
			return ePropType::BROKENSUBMARINE_A;
		}
		else if (aType == "sm_car_a")
		{
			return ePropType::CAR_A;
		}
		else if (aType == "sm_fence_a")
		{
			return ePropType::FENCE_A;
		}
		else if (aType == "sm_fence_b")
		{
			return ePropType::FENCE_B;
		}
		else if (aType == "sm_junkpile_a")
		{
			return ePropType::JUNKPILE_A;
		}
		else if (aType == "sm_junkpile_b")
		{
			return ePropType::JUNKPILE_B;
		}
		else if (aType == "sm_log_a")
		{
			return ePropType::LOG_A;
		}
		else if (aType == "sm_log_grp")
		{
			return ePropType::LOG_GROUP;
		}
		else if (aType == "sm_oldpier_a")
		{
			return ePropType::OLDPIER_A;
		}
		else if (aType == "sm_pipes_a")
		{
			return ePropType::PIPES_A;
		}
		else if (aType == "sm_pipes_b")
		{
			return ePropType::PIPES_B;
		}
		else if (aType == "sm_rubble_a")
		{
			return ePropType::RUBBLE_A;
		}
		else if (aType == "sm_rubble_b")
		{
			return ePropType::RUBBLE_B;
		}
		else if (aType == "sm_rubble_c")
		{
			return ePropType::RUBBLE_C;
		}
		else if (aType == "sm_rubble_d")
		{
			return ePropType::RUBBLE_D;
		}
		else if (aType == "sm_tank_a")
		{
			return ePropType::TANK_A;
		}
		else if (aType == "sm_tent_a")
		{
			return ePropType::TENT_A;
		}
		else if (aType == "sm_tent_b")
		{
			return ePropType::TENT_B;
		}
		else if (aType == "sm_wheels_a")
		{
			return ePropType::WHEELS_A;
		}
		else if (aType == "sm_wheels_b")
		{
			return ePropType::WHEELS_B;
		}
		else if (aType == "sm_electric_post_a")
		{
			return ePropType::ELECTRIC_POST_A;
		}
		DL_ASSERT("The " + aType + " prop type is not supported, please tell Daniel about it.");
		return ePropType::NOT_A_PROP;
	}

	const eOwnerType ConvertStringToOwnerType(const std::string& aType)
	{
		if (aType == "player")
		{
			return eOwnerType::PLAYER;
		}
		else if (aType == "enemy")
		{
			return eOwnerType::ENEMY;
		}
		else if (aType == "neutral")
		{
			return eOwnerType::NEUTRAL;
		}
		DL_ASSERT("The " + aType + " owner type is not supported, please tell Daniel about it.");
		return eOwnerType::NOT_USED;
	}
}