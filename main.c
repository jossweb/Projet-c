#include "raylib.h"
#include <math.h>

#define WIDTH 1400
#define HEIGHT 850
#define PLAYERSCALE 0.35f

// Global textures
Texture2D missionaryTexture;
Texture2D cannibalTexture;
Texture2D boatTexture;
Texture2D backgroundTexture;

struct Player{
    Vector2 position;
    int type; // 0 = missionary; 1 = cannibal
};
typedef struct Player Player;

struct Pile {
    Player p;
    Player *next;
};
typedef struct Pile Pile;

struct Boat {
    Vector2 position;
    float speed;
    Pile onBoardList;
};
typedef struct Boat Boat;

void LoadGameTextures() {
    missionaryTexture = LoadTexture("assets/missionnaire.png");
    cannibalTexture = LoadTexture("assets/cannibale.png");
    boatTexture = LoadTexture("assets/bateau.png");
    backgroundTexture = LoadTexture("assets/fond.png");
}

void UnloadGameTextures() {
    UnloadTexture(missionaryTexture);
    UnloadTexture(cannibalTexture);
    UnloadTexture(boatTexture);
    UnloadTexture(backgroundTexture);
}

Player createplayer(int type, int x, int y){
    Player p;
    p.position.x = x;
    p.position.y = y;
    p.type = type;
    return p;
}

void printPlayer(Player p, float scale){
    Texture2D *texture = (p.type == 0) ? &missionaryTexture : &cannibalTexture;
    DrawTextureEx(*texture, (Vector2){p.position.x, p.position.y}, 0.0f, scale, WHITE);
}

Boat InitBoat(){
    Boat b;
    b.position.x = 700;
    b.position.y = 450;
    b.speed = 2;
    return b;
}

void printBoat(Boat b, float scale){
    static float waveTime = 0.0f;
    waveTime += GetFrameTime();
    float waveOffset = 5.0f * sinf(waveTime * 2.0f);

    DrawTextureEx(
        boatTexture,
        (Vector2){ b.position.x, b.position.y + waveOffset },
        0.0f,
        scale,
        WHITE
    );
}


void GameSystem(){
    //Create humain and print them
    Player human1 = createplayer(0, 1250, 400);
    Player human2 = createplayer(0, 1175, 400);
    Player human3 = createplayer(0, 1100, 400);
    printPlayer(human1, PLAYERSCALE);
    printPlayer(human2, PLAYERSCALE);
    printPlayer(human3, PLAYERSCALE);

    //Create cannibals and print them
    Player canibal1 = createplayer(1, 1200, 500);
    Player canibal2 = createplayer(1, 1125, 500);
    Player canibal3 = createplayer(1, 1050, 500);
    printPlayer(canibal1, PLAYERSCALE);
    printPlayer(canibal2, PLAYERSCALE);
    printPlayer(canibal3, PLAYERSCALE);

    //Create boat and print it
    Boat boat = InitBoat();
    printBoat(boat, 0.40f);
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Projet C par Rémy.M et Jossua.F");
    
    LoadGameTextures();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        
        float scale = (float)(WIDTH) / (float)(backgroundTexture.width);
        float posX = (WIDTH - (backgroundTexture.width * scale)) * 0.5f;
        float posY = (HEIGHT - (backgroundTexture.height * scale)) * 0.5f;
        
        DrawTextureEx(backgroundTexture, (Vector2){posX, posY}, 0.0f, scale, WHITE);
        GameSystem();
        EndDrawing();
    }

    UnloadGameTextures();
    CloseWindow();
    return 0;
}