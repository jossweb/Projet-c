#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"

#define WIDTH 1400
#define HEIGHT 850
#define PLAYERSCALE 0.35f
#define BOATSTARTX 725


typedef enum {
    STATE_INTRO,
    STATE_PILE,
    STATE_FILE
} GameState;

// Global textures
Texture2D missionaryTexture;
Texture2D cannibalTexture;
Texture2D boatTexture;
Texture2D IntrobackgroundTexture;
Texture2D backgroundTexture;
Texture2D missionarySeatedTexture;
Texture2D cannibalSeatedTexture;
//here we use extern term for have global variable only one time in the program

extern GameState current;
extern int gamePaused;
extern float animationSpeedMultiplier;
extern Rectangle speedSlider;
extern int sliderActive;


struct Player{
    Vector2 position;
    int type; // 0 = missionary; 1 = cannibal
    Vector2 destination;
    int onMove; // 0 = not moving; 1 = moving
};
typedef struct Player Player;

struct Boat {
    Vector2 position;
    int location; // 0 = moving; 1 = start; 2 = end
    Vector2 destination;
};
typedef struct Boat Boat;

//global entities
Player human1;
Player human2;
Player human3;
Player canibal1;
Player canibal2;
Player canibal3;
Boat boat;

//array
Player *players[6];

//Location of player in boat
Vector2 boatPosition[2];

#endif