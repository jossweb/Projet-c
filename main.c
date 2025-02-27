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
    int onMove; // 0 = no; 1 = yes
    Vector2 stopPosition;
};
typedef struct Player Player;

struct Pile {
    Player *p;
    struct Pile *prev;
};
typedef struct Pile Pile;

struct Boat {
    Vector2 position;
    int State; // 0 = on move; 1 = stopped at start; 2 = stopped at end
    int newState; // 0 = State Value don't change since last frame; 1 = State Value changed since last frame
};
typedef struct Boat Boat;

//global entities
Player human1;
Player human2;
Player human3;
Player canibal1;
Player canibal2;
Player canibal3;
//player list
Player* players[6];
//global boat
Boat boat;
//global piles
Pile startList;
Pile inTravelList;
Pile endList;

// loading game functions
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

// game functions
int PileSize(Pile p, int size){
    if(p.p == NULL){
        return size;
    }
    if(p.prev == NULL) {
        return size + 1;
    }
    return PileSize(*p.prev, size + 1);
}
Pile *InitStartPile(Pile *pileStart, int i) {
    if (i >= 6) {
        return pileStart;
    }

    Pile *newPile = (Pile *)malloc(sizeof(Pile));
    newPile->p = players[i];
    newPile->prev = pileStart;

    printf("InitStartPile --------------------- %d\n", PileSize(*newPile, 0));

    return InitStartPile(newPile, i + 1);
}
Player createplayer(int type, int x, int y){
    Player p;
    p.position.x = x;
    p.position.y = y;
    p.type = type;
    p.onMove = 0;
    p.stopPosition.x = 0;
    p.stopPosition.y = 0;
    return p;
}

void printPlayer(Player p, float scale){
    Texture2D *texture = (p.type == 0) ? &missionaryTexture : &cannibalTexture;
    DrawTextureEx(*texture, (Vector2){p.position.x, p.position.y}, 0.0f, scale, WHITE);
}

void InitBoat(){
    boat.position.x = BOATSTARTX;
    boat.position.y = 450;
    boat.State= 1; //initial position 
    boat.newState = 0;    
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
void InitPile(Pile *p){
    p->p= NULL;
    p->prev = NULL;
}
void PrintEntities(){
    // Create humans
   for(int i = 0; i < 6; i++){
        printPlayer(*players[i], PLAYERSCALE);
    }

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
    InitBoat();
}
void movePlayer(Player *p){
    if (p->onMove) {
        Vector2 direction = {
            p->stopPosition.x - p->position.x,
            p->stopPosition.y - p->position.y
        };
        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

        if (distance < 1.0f) { // Seuil de tolérance
            p->position = p->stopPosition;
            p->onMove = 0;
        } else {
            direction.x /= distance;
            direction.y /= distance;
            const float speed = 2.0f; 
            p->position.x += direction.x * speed;
            p->position.y += direction.y * speed;
        }
    }

    printPlayer(*p, PLAYERSCALE);
}
void InitTable() {
    players[0] = &human1;
    players[1] = &human2;
    players[2] = &human3;
    players[3] = &canibal1;
    players[4] = &canibal2;
    players[5] = &canibal3;
}
void FirstInit() {
    InitPile(&startList);
    InitPile(&inTravelList);
    InitPile(&endList);
    Pile *initializedPile = InitStartPile(&startList, 0);
    startList = *initializedPile;
}
void PopPile(Pile *p){
    if(p->prev == NULL){
        p->p = NULL;
    }else{
        p->p = NULL;
        p = p->prev;
    }
}
void game(){
    printf("game --------------------- %d\n", PileSize(startList, 0));
    printf("boat state : %d\n", boat.State);
    int playermoving = 0; // 1 = 1 player (or more) is moving; 0 = no player is moving
    for(int i = 0; i < 6; i++){
        printf("in\n");
        if(players[i]->onMove){
            playermoving = 1;
        }
    }
    if(playermoving){
        printf("player moving ---------------------\n");
        for(int i = 0; i < 6; i++){
            if(players[i]->onMove){
                printf("x : %d, y : %d\n", (int)players[i]->position.x, (int)players[i]->position.y);
                movePlayer(players[i]);
            }
        }
    }
    else if(boat.State == 0){
        // exécuter le code pour déplacer le bateau
    }
    else if(boat.State == 1){
        printf("good boat state ---------------------\n Pile size : %d\n", PileSize(startList, 0));
        if (PileSize(startList, 0) == 6) { // First move
            printf("first move ---------------------\n");
            if (startList.p != NULL) {
                startList.p->onMove = 1;
                startList.p->stopPosition.x = boat.position.x;
                startList.p->stopPosition.y = boat.position.y;
                movePlayer(startList.p);
                PopPile(&startList);
            }
            if (startList.p != NULL) {
                startList.p->onMove = 1;
                startList.p->stopPosition.x = boat.position.x;
                startList.p->stopPosition.y = boat.position.y;
                movePlayer(startList.p);
                PopPile(&startList);
            }
        }
        else{
            // exécuter le code pour déplacer les joueurs
        }
    }
    else if(boat.State == 2 && boat.newState == 1){
        // exécuter le code pour déplacer les joueurs
    }
}
int main(void)
{
    InitEntites();
    InitWindow(WIDTH, HEIGHT, "Projet C par Rémy.M et Jossua.F");
    FirstInit();
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
            game();
            PrintEntities();
            EndDrawing();
        }

    UnloadGameTextures();
    CloseWindow();
    return 0;
}