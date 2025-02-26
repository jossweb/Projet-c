#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
Texture2D backgroundTexture;
GameState current = STATE_INTRO;


struct Player{
    Vector2 position;
    int type; // 0 = missionary; 1 = cannibal
};
typedef struct Player Player;

struct Pile {
    Player *p;
    struct Pile *next;
};
typedef struct Pile Pile;

struct Boat {
    Vector2 position;
    float speed;
    Pile onBoardList;
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
    b.position.x = BOATSTARTX;
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

void DrawIntro() {
    // Background
    ClearBackground(BLACK);
    float scale = (float)(WIDTH) / (float)(backgroundTexture.width);
    float posX = (WIDTH - (backgroundTexture.width * scale)) * 0.5f;
    float posY = (HEIGHT - (backgroundTexture.height * scale)) * 0.5f;
    DrawTextureEx(backgroundTexture, (Vector2){posX, posY}, 0.0f, scale, WHITE);

    // Text
    float TitleSize = HEIGHT * 0.03f;
    float TitleY = HEIGHT * 0.12f;
    float CreditSize = HEIGHT * 0.02f;

    const char* title = "Problème des Missionnaires et des Cannibales";
    float titleWidth = MeasureText(title, TitleSize);
    
    DrawText(title, (WIDTH - titleWidth), TitleY, TitleSize, BLACK);

    
}
int PileSize(Pile p, int size){
    if(p.p == NULL){
        return size;
    }
    if(p.next == NULL) {
        return size + 1;
    }
    return PileSize(*p.next, size + 1);
}
void InitPile(Pile *p){
    p->p= NULL;
    p->next = NULL;
}
void PrintEntities(){
    // Create humans
    printPlayer(human1, PLAYERSCALE);
    printPlayer(human2, PLAYERSCALE);
    printPlayer(human3, PLAYERSCALE);

    // Create canibals
    printPlayer(canibal1, PLAYERSCALE);
    printPlayer(canibal2, PLAYERSCALE);
    printPlayer(canibal3, PLAYERSCALE);

    // Create boat and print it
    printBoat(boat, PLAYERSCALE);
}
void InitEntites(){
    human1 = createplayer(0, 1250, 400);
    human2 = createplayer(0, 1175, 400);
    human3 = createplayer(0, 1100, 400);

    // Create cannibals and print them
    canibal1 = createplayer(1, 1200, 500);
    canibal2 = createplayer(1, 1125, 500);
    canibal3 = createplayer(1, 1050, 500);

    //init boat
    boat = InitBoat();
}
int main(void)
{
    InitEntites();
    InitWindow(WIDTH, HEIGHT, "Projet C par Rémy.M et Jossua.F");
    
    LoadGameTextures();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        switch(current) {
            case STATE_INTRO:
                DrawIntro();
                break;
            case STATE_PILE:
                ClearBackground(BLACK);
                
                float scale = (float)(WIDTH) / (float)(backgroundTexture.width);
                float posX = (WIDTH - (backgroundTexture.width * scale)) * 0.5f;
                float posY = (HEIGHT - (backgroundTexture.height * scale)) * 0.5f;
                
                DrawTextureEx(backgroundTexture, (Vector2){posX, posY}, 0.0f, scale, WHITE);
            }
            PrintEntities();
            EndDrawing();
        }

    UnloadGameTextures();
    CloseWindow();
    return 0;
}