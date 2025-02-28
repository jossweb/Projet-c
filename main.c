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
    Vector2 destination;
    int onMove; // 0 = not moving; 1 = moving
};
typedef struct Player Player;

struct Pile {
    Player *p;
    struct Pile *prev;
};
typedef struct Pile Pile;

struct Boat {
    Vector2 position;
    int onMove; // 0 = not moving; 1 = moving
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

//piles
Pile startPile;
Pile onBoat;
Pile endPile;

//array
Player *players[6];

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
    p.onMove = 0;
    p.destination = p.position;
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
    b.onMove = 0;
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
    if(p.prev == NULL) {
        return size + 1;
    }
    return PileSize(*p.prev, size + 1);
}
void InitPile(Pile *p){
    p->p= NULL;
    p->prev = NULL;
}
void PrintEntities(){
    // Create players
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
void InitStartPile(Player *players[]) {
    Pile *current = &startPile;
    for (int i = 0; i < 6; i++) {
        current->p = players[i];
        if (i < 5) {
            current->prev = malloc(sizeof(Pile));
            current = current->prev;
        } else {
            current->prev = NULL;
        }
    }
}

void PrintPileTypes(Pile p) {
    if(p.p == NULL) {
        printf("Pile is empty\n");
        return;
    }
    
    printf("Player type: %d\n", p.p->type);
    
    if(p.prev != NULL) {
        PrintPileTypes(*p.prev);
    }
}
void InitEntites(){
    human1 = createplayer(0, 1250, 400);
    human2 = createplayer(0, 1175, 400);
    human3 = createplayer(0, 1100, 400);

    // Create cannibals and print them
    canibal1 = createplayer(1, 1200, 500);
    canibal2 = createplayer(1, 1125, 500);
    canibal3 = createplayer(1, 1050, 500);

    // Add players to the global array
    players[0] = &human1;
    players[1] = &human2;
    players[2] = &human3;
    players[3] = &canibal1;
    players[4] = &canibal2;
    players[5] = &canibal3;

    // Initialize start pile
    InitStartPile(players);
    PrintPileTypes(startPile);

    // Initialize boat
    boat = InitBoat();
}
int movePlayers() {
    int moved = 0; // 0 = no player moved; 1 = 1 player (or more) moved
    for (int i = 0; i < 6; i++) {
        Player *p = players[i];
        if (p->onMove) {
            Vector2 direction = {
                p->destination.x - p->position.x,
                p->destination.y - p->position.y
            };
            float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

            if (distance < 1.0f) { // Seuil de tolérance
                p->position = p->destination;
                p->onMove = 0;
            } else {
                direction.x /= distance;
                direction.y /= distance;
                const float speed = 2.0f; 
                p->position.x += direction.x * speed;
                p->position.y += direction.y * speed;
                moved = 1;
            }
        }
    }
    return moved;
}
void FromStartToBoat(){
    if(PileSize(startPile, 0) > 0){
        Pile *temp = startPile.prev;
        startPile.prev = onBoat.prev;
        onBoat.prev = &startPile;
        startPile = *temp;
    }
}
void game(){
    if(boat.onMove){
        //faire avancer le bateau
    }
    else{
        if(!movePlayers()){
            if(PileSize(startPile, 0) == 6){
                //starting scenario
                for(int i=0; i<2; i++){ //goal here : take 2 players from start pile to boat
                    startPile.p->onMove=1;
                    startPile.p->destination = boat.position;
                    FromStartToBoat();
                }
            }
        }
    }
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
            printf("Start Game -----------------");
            game();
            EndDrawing();
        }

    UnloadGameTextures();
    CloseWindow();
    return 0;
}