#include "stdafx.h"
#include "GameConstants.h"

bool GC::GenerateLightData = false; 
bool GC::GenerateCubeMap = false;
int GC::SHNodeSize = 4;

bool GC::EnableCheapAmbient = false;
bool GC::EnableVSync = true;

int GC::Difficulty = 10;

float GC::DamageMultiplier[3];
float GC::SpawnIntervalMultiplier[3];

volatile bool GC::PlayerAlive = true;

volatile bool GC::PlayerShouldPlaySprintErrorSound = false;

GC::eMultiplayerMode GC::MultiplayerMode = GC::eMultiplayerMode::HOST;