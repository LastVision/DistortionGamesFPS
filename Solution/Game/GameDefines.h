#pragma once

#define AMOUNT_OF_CONTROL_GROUPS 9
#define AMOUNT_OF_UPGRADES 3
#define PLAYER_COLOR { 0.f, 0.35f, 0.75f, 1.f }

//#define USE_SHADOW
#define LOAD_PROPS
//#define CLICK_EXPLOSION
#define USE_DIFFICULTY


#ifdef _DEBUG

#undef LOAD_PROPS
#undef USE_SHADOW

const bool globalUseParticles = false;
const bool globalUseWeather = false;

#else

const bool globalUseParticles = true;
const bool globalUseWeather = true;

#endif