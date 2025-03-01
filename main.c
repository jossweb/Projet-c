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
Texture2D IntrobackgroundTexture;
Texture2D backgroundTexture;
GameState current = STATE_INTRO;
int gamePaused = 0;


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

//piles
Pile startPile;
Pile onBoat;
Pile endPile;

//array
Player *players[6];

//Location of player in boat
Vector2 boatPosition[2];

void LoadGameTextures() {
    missionaryTexture = LoadTexture("assets/missionnaire.png");
    cannibalTexture = LoadTexture("assets/cannibale.png");
    boatTexture = LoadTexture("assets/bateau.png");
    IntrobackgroundTexture = LoadTexture("assets/intro.png");
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
    b.location = 1;
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
    float scale = (float)(WIDTH) / (float)(IntrobackgroundTexture.width);
    float posX = (WIDTH - (IntrobackgroundTexture.width * scale)) * 0.5f;
    float posY = (HEIGHT - (IntrobackgroundTexture.height * scale)) * 0.5f;
    DrawTextureEx(IntrobackgroundTexture, (Vector2){posX, posY}, 0.0f, scale, WHITE);
    
    // Button properties
    const int buttonWidth = 200;
    const int buttonHeight = 60;
    const int buttonGap = 50;  // Gap between buttons
    const int fontSize = 24;
    
    // Center buttons both horizontally and vertically
    int pileButtonX = WIDTH/2 - buttonWidth - buttonGap/2;
    int fileButtonX = WIDTH/2 + buttonGap/2;
    int buttonY = HEIGHT/2 + 100;  // Move buttons down to center vertically
    
    // Draw "Pile" button
    Rectangle pileButton = { pileButtonX, buttonY, buttonWidth, buttonHeight };
    Color pileButtonColor = LIGHTGRAY;
    Vector2 mousePos = GetMousePosition();
    
    if (CheckCollisionPointRec(mousePos, pileButton)) {
        pileButtonColor = GRAY;  // Darker when hovered
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            gamePaused = 0;
            current = STATE_PILE;
        }
    }
    
    DrawRectangleRec(pileButton, pileButtonColor);
    DrawRectangleLinesEx(pileButton, 2, BLACK);  // Add border
    DrawText("Pile", pileButtonX + buttonWidth/2 - MeasureText("Pile", fontSize)/2, 
             buttonY + buttonHeight/2 - fontSize/2, fontSize, BLACK);
    
    // Draw "File" button
    Rectangle fileButton = { fileButtonX, buttonY, buttonWidth, buttonHeight };
    Color fileButtonColor = LIGHTGRAY;
    
    if (CheckCollisionPointRec(mousePos, fileButton)) {
        fileButtonColor = GRAY;  // Darker when hovered
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            gamePaused = 0;
            current = STATE_FILE;
        }
    }
    
    DrawRectangleRec(fileButton, fileButtonColor);
    DrawRectangleLinesEx(fileButton, 2, BLACK);  // Add border
    DrawText("File", fileButtonX + buttonWidth/2 - MeasureText("File", fontSize)/2, 
             buttonY + buttonHeight/2 - fontSize/2, fontSize, BLACK);
}

void DrawGameControls() {
    // Back button
    Rectangle backButton = { 20, 20, 100, 40 };
    Color backButtonColor = LIGHTGRAY;
    Vector2 mousePos = GetMousePosition();
    
    if (CheckCollisionPointRec(mousePos, backButton)) {
        backButtonColor = GRAY;  // Darker when hovered
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            current = STATE_INTRO;
        }
    }
    
    DrawRectangleRec(backButton, backButtonColor);
    DrawRectangleLinesEx(backButton, 2, BLACK);
    DrawText("Back", backButton.x + backButton.width/2 - MeasureText("Back", 20)/2,
             backButton.y + backButton.height/2 - 10, 20, BLACK);
    
    // Play/Pause button
    Rectangle pauseButton = { 140, 20, 100, 40 };
    Color pauseButtonColor = LIGHTGRAY;
    
    if (CheckCollisionPointRec(mousePos, pauseButton)) {
        pauseButtonColor = GRAY;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            gamePaused = !gamePaused;
        }
    }
    
    DrawRectangleRec(pauseButton, pauseButtonColor);
    DrawRectangleLinesEx(pauseButton, 2, BLACK);
    DrawText(gamePaused ? "Play" : "Pause", 
             pauseButton.x + pauseButton.width/2 - MeasureText(gamePaused ? "Play" : "Pause", 20)/2,
             pauseButton.y + pauseButton.height/2 - 10, 20, BLACK);
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
    players[0] = &canibal1;
    players[1] = &human1;
    players[2] = &canibal2;
    players[3] = &human3;
    players[4] = &human2;
    players[5] = &canibal3;

    // Initialize start pile
    InitStartPile(players);
    PrintPileTypes(startPile);

    // Initialize boat
    boat = InitBoat();
    InitPile(&onBoat);
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

            if (distance < 2.0f) {
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
void moveBoat() {
    if (boat.location == 0){
        Vector2 direction = {
            boat.destination.x - boat.position.x,
            boat.destination.y - boat.position.y
        };
        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

        if (distance < 2.0f) {
            boat.position = boat.destination;
            if(boat.destination.x == BOATSTARTX){
                boat.location = 1;
            } else {
                boat.location = 3;
            }
        } else {
            direction.x /= distance;
            direction.y /= distance;
            const float speed = 2.0f; 
            boat.position.x += direction.x * speed;
            boat.position.y += direction.y * speed;
        }
    }
}
void FromStartToBoat(){
    if(PileSize(startPile, 0) > 0){
        // Sauvegarde l'élément du sommet
        Pile *movedPile = malloc(sizeof(Pile));
        *movedPile = startPile;
        movedPile->prev = NULL;

        // Retire l'élément du sommet de startPile
        if (startPile.prev == NULL) {
            InitPile(&startPile);
        } else {
            startPile = *startPile.prev;
        }

        // Ajoute l'élément au sommet de onBoat
        if (onBoat.p == NULL) {
            onBoat.p = movedPile->p;
            onBoat.prev = movedPile->prev;
            free(movedPile);
        } else {
            Pile *oldTop = malloc(sizeof(Pile));
            *oldTop = onBoat;

            onBoat = *movedPile;
            onBoat.prev = oldTop;
            free(movedPile);
        }
    }
}
// void FromStartToBoat(){
//     if(PileSize(startPile, 0) > 0){
//         Pile *temp = startPile.prev;
//         Pile *movedPile = malloc(sizeof(Pile));
//         *movedPile = startPile;
//         movedPile->prev = NULL;
//         if (onBoat.p == NULL) {
//             printf("1-------\n");
//             onBoat.p = movedPile->p;
//             onBoat.prev = movedPile->prev;
//             free(movedPile);
//         } else {
//             printf("2-------\n");
//             Pile *current = &onBoat;
//             while (current->prev != NULL) {
//                 current = current->prev;
//             }
//             current->prev = movedPile;
//         }
//         startPile = *temp;
//     }
// }

void FromBoatToEnd() {
    if(PileSize(onBoat, 0) > 0) {
        Pile *movedPile = malloc(sizeof(Pile));
        *movedPile = onBoat;
        movedPile->prev = NULL;

        if(onBoat.prev == NULL) {
            InitPile(&onBoat); 
        } else {
            onBoat = *onBoat.prev;
        }

        if(endPile.p == NULL) {
            // endPile est vide : on met directement l'élément
            endPile.p = movedPile->p;
            endPile.prev = movedPile->prev;
            free(movedPile);
        } else {
            // endPile n'est pas vide : on empile au sommet
            Pile *oldTop = malloc(sizeof(Pile));
            *oldTop = endPile;
            endPile = *movedPile; 
            endPile.prev = oldTop;
            free(movedPile);
        }
    }
}
// void FromBoatToEnd(){
//     if(PileSize(onBoat, 0) > 0){
//         Pile *temp = onBoat.prev;
//         Pile *movedPile = malloc(sizeof(Pile));
//         *movedPile = onBoat;
//         movedPile->prev = NULL;

//         // Ajouter l'élément à endPile
//         if (endPile.p == NULL) {
//             endPile.p = movedPile->p;
//             endPile.prev = movedPile->prev;
//             free(movedPile);
//         } else {
//             movedPile->prev = endPile.prev;
//             endPile.prev = movedPile;
//         }

//         onBoat = *temp; // Mettre à jour onBoat
//     }
// }

void SetDestinationToPlayerOnBoat(Vector2 destinationP1, Vector2 destinationP2){
    Pile *currentOnBoat = &onBoat; //Take pointer for set modifications out of the fonction
    if(currentOnBoat != NULL) {
        // Set destination to the first player
        currentOnBoat->p->destination = destinationP1;
        currentOnBoat->p->onMove = 1;
        currentOnBoat = currentOnBoat->prev;
        if (currentOnBoat != NULL) { //check if there is a second player on boat
            // Set destination to the second player
            currentOnBoat->p->destination = destinationP2;
            currentOnBoat->p->onMove = 1;
        }
    }
}
void updatePlayersPositonsInBoat(){
    boatPosition[0].x = boat.position.x + 75;
    boatPosition[0].y = boat.position.y - 100;
    boatPosition[1].x = boat.position.x + 25;
    boatPosition[1].y = boat.position.y - 100;
}
void game(){
    if(boat.location == 0){
        moveBoat();
        if(!movePlayers()){
            printf("WARNING : Boat is moving without player(s)");
        }
    }
    else if(boat.location == 1){
        if(!movePlayers()){
            if(PileSize(onBoat, 0) == 2){
                boat.location = 0;
                boat.destination.x = 400;
                boat.destination.y = 450;
                Vector2 destinationP1 = {boat.destination.x + 110, boat.position.y - 110};
                Vector2 destinationP2 = {boat.destination.x + 50, boat.position.y - 110};
                SetDestinationToPlayerOnBoat(destinationP1, destinationP2);

            }
            if(PileSize(startPile, 0) == 6){
                //starting scenario
                for(int i=0; i<2; i++){ //goal here : take 2 players from start pile to boat
                    startPile.p->onMove=1;
                    updatePlayersPositonsInBoat();
                    startPile.p->destination = boatPosition[i];
                    FromStartToBoat();
                }
            }
            if((PileSize(startPile, 0) < 6)&&(PileSize(onBoat, 0) == 1)){
                startPile.p->onMove=1;
                updatePlayersPositonsInBoat();
                startPile.p->destination = boatPosition[1];
                FromStartToBoat();
            }
        }
    } else { // boat at end
        if (onBoat.p != NULL) {
            printf("on move : %d\n", onBoat.p->onMove);
        } else {
            printf("onBoat is empty\n");
        }
        printf("at end : %d\n", PileSize(endPile, 0));
        if(!movePlayers()){
            printf("end ----- %d\n", PileSize(onBoat, 0));
            if(PileSize(onBoat, 0) == 2){
                Vector2 finalDestination;
                if(!PileSize(endPile, 0)){
                    finalDestination.x = 0;
                    finalDestination.y = 350;
                }
                else{
                    finalDestination = endPile.p->position;
                    finalDestination.x += 75;
                }
                FromBoatToEnd();
                endPile.p->onMove = 1;
                endPile.p->destination = finalDestination;
                if(!PileSize(startPile, 0)){ //last situation
                    FromBoatToEnd();
                    endPile.p->onMove = 1;
                    endPile.p->destination = finalDestination;
                }
            }else{
                if(startPile.p){
                    boat.location = 0;
                    boat.destination.x = BOATSTARTX;
                    boat.destination.y = 450;
                    onBoat.p->onMove = 1;
                    Vector2 destination = {boat.destination.x + 50, boat.position.y - 110};
                    onBoat.p->destination = destination;
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
    SetTargetFPS(120);

    while (!WindowShouldClose()){
        BeginDrawing();

        switch(current) {
            case STATE_INTRO:
                ClearBackground(BLACK);
                DrawIntro();
                break;
            case STATE_PILE:
                ClearBackground(BLACK);
                    
                float scale = (float)(WIDTH) / (float)(backgroundTexture.width);
                float posX = (WIDTH - (backgroundTexture.width * scale)) * 0.5f;
                float posY = (HEIGHT - (backgroundTexture.height * scale)) * 0.5f;
                    
                DrawTextureEx(backgroundTexture, (Vector2){posX, posY}, 0.0f, scale, WHITE);
                PrintEntities();

                if (!gamePaused) {
                    game();
                }
                
                DrawGameControls();
                break;
            case STATE_FILE:
                ClearBackground(BLACK);
                DrawGameControls();
                break;
        }
        EndDrawing();
    }

    UnloadGameTextures();
    CloseWindow();
    return 0;
}