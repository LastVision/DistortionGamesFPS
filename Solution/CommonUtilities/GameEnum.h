#pragma once

enum class eGameType : unsigned char
{
	SINGLEPLAYER,
	MULTIPLAYER,
};

enum class eWeaponType
{
	PISTOL,
	SHOTGUN,
	GRENADE_LAUNCHER
};

enum class eMissionType
{
	NONE,
	KILL_X,
	DEFEND,
	EVENT,
};


enum class eActionEventType
{
	LOCK,
	UNLOCK,
	TEXT,
	SPAWN,
	MARKER,
};

enum class eInGameStates
{
	LEVEL_UPDATE,
	LEVEL_COMPLETE,
	LEVEL_COMPLETE_ALL_CLIENTS_RESPONDED,
	LEVEL_LOAD,
	LEVEL_WAIT_FOR_OTHER,
};

enum class eObjectRoomType
{
	NONE,
	NOT_USED_ON_SERVER,
	ALWAYS_RENDER,
	DYNAMIC,
	STATIC,
};

enum eOwnerType
{
	NOT_USED = -1,
	ENEMY = 1,
	NEUTRAL = 2
};

enum eEntityType
{
	EMPTY = -1,
	UNIT = 1,
	PROP = 2,
	GRENADE = 4,
	TRIGGER = 8,
	PLAYER = 16,
	EXPLOSION = 32,
	BULLET = 64,
	SPAWNPOINT = 128,
	_COUNT,
};

enum class eTriggerType : int
{
	EMPTY = -1,
	LEVEL_CHANGE,
	HEALTH_PACK,
	UPGRADE,
	UNLOCK,
	LOCK,
	MISSION,
	RESPAWN,
	ENEMY_SPAWN,
	MARKER,
	_COUNT,
};

enum class ePhysics
{
	STATIC,
	DYNAMIC,
	PHANTOM,
	CAPSULE,
	KINEMATIC,
};

enum class ePropType
{
	NOT_A_PROP,
	PINE_TREE_A,
	PINE_TREE_B,
	PINE_TREE_C,
	PINE_TREE_D,
	BIRCH_TREE_A,
	HOUSE_SMALL,
	HOUSE_MEDIUM,
	HOUSE_LARGE,
	HOUSE_GROUP,
	RESOURCE_POLE,
	VICTORY_POLE,
	BUSH_A,
	BUSH_B,
	BUSH_C,
	ELECTRIC_POST_A,
	ROCK_LARGE,
	ROCK_MEDIUM,
	ROCK_SMALL,
	ROCKS_GROUP_A,
	ROCKS_GROUP_B,
	BONES_MEDIUM,
	BONES_SMALL,
	BROKENSHIP_A,
	BROKENSHIP_B,
	BROKENSUBMARINE_A,
	CAR_A,
	FENCE_A,
	FENCE_B,
	JUNKPILE_A,
	JUNKPILE_B,
	LOG_A,
	LOG_GROUP,
	OLDPIER_A,
	PIPES_A,
	PIPES_B,
	RUBBLE_A,
	RUBBLE_B,
	RUBBLE_C,
	RUBBLE_D,
	TANK_A,
	TENT_A,
	TENT_B,
	WHEELS_A,
	WHEELS_B

};

enum class eEntityState : unsigned char
{
	IDLE,
	WALK,
	ATTACK,
	DIE,
	_COUNT,
};

enum class ePlayerState : int
{
	PISTOL_IDLE,
	PISTOL_FIRE,
	PISTOL_RELOAD,
	PISTOL_DRAW,
	PISTOL_HOLSTER,
	SHOTGUN_IDLE,
	SHOTGUN_FIRE,
	SHOTGUN_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	GRENADE_LAUNCHER_IDLE,
	GRENADE_LAUNCHER_FIRE,
	GRENADE_LAUNCHER_RELOAD,
	GRENADE_LAUNCHER_DRAW,
	GRENADE_LAUNCHER_HOLSTER,
	_COUNT,
};

enum class eNetRayCastType : int
{
	CLIENT_SHOOT_PISTOL,
	CLIENT_SHOOT_SHOTGUN,
	_COUNT,
};

enum class eComponentType
{
	NOT_USED = -1,
	ANIMATION,
	GRAPHICS,
	HEALTH,
	NETWORK,
	GRENADE,
	TRIGGER,
	INPUT,
	SHOOTING,
	FIRST_PERSON_RENDER,
	PHYSICS,
	UPGRADE,
	AI,
	BULLET,
	SPAWNPOINT,
	SOUND,
	ROTATION,
	_COUNT,
};

#undef ERROR
enum class eHistoryType
{
	ERROR,
	HISTORY,
	HELP,
	GENERATED_COMMAND,
	WARNING
};

enum class eFuzzyAI
{
	SPAWN_GRUNT,
	SPAWN_RANGER,
	SPAWN_TANK,
	TAKE_RESOURCE_POINT,
	TAKE_VICTORY_POINT,
	TAKE_ARTIFACT,
	UPGRADE_GRUNT,
	UPGRADE_RANGER,
	UPGRADE_TANK,
	ROAM,
	DO_NOTHING,
	_COUNT,
};

enum class eCursorType
{
	NORMAL
};

enum class eDifficulty
{
	EASY,
	NORMAL,
	HARD
};