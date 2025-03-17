#include "common.h"

GameState current = STATE_INTRO;
int gamePaused = 0;
float animationSpeedMultiplier = 1.0f;
Rectangle speedSlider = { 0 };
int sliderActive = 0;