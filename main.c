#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "pile.h"
#include "common.h"
#include "file.h"

void LoadGameTextures();
void UnloadGameTextures();
Player createplayer(int type, int x, int y);
void printPlayer(Player p, float scale);
Boat InitBoat();
void printBoat(Boat b, float scale);
void DrawIntro();
void DrawSpeedSlider();
void DrawGameControls();
void PrintEntities();
void InitEntites();
int movePlayers();
void moveBoat();
void FromStartToBoat();
void FromBoatToEnd();
void SetDestinationToPlayerOnBoat(Vector2 destinationP1, Vector2 destinationP2);
void updatePlayersPositonsInBoat();
void game();
void BoatToWaiting(int boatSize);
File GetLast(File file);  

void LoadGameTextures() {
    missionaryTexture = LoadTexture("assets/missionnaire.png");
    cannibalTexture = LoadTexture("assets/cannibale.png");
    boatTexture = LoadTexture("assets/bateau.png");
    IntrobackgroundTexture = LoadTexture("assets/intro.png");
    backgroundTexture = LoadTexture("assets/fond.png");
    // nouvelle texture assis
    missionarySeatedTexture = LoadTexture("assets/missionnaire_a.png");
    cannibalSeatedTexture = LoadTexture("assets/cannibale_a.png");
}

void UnloadGameTextures() {
    UnloadTexture(missionaryTexture);
    UnloadTexture(cannibalTexture);
    UnloadTexture(boatTexture);
    UnloadTexture(backgroundTexture);
    UnloadTexture(IntrobackgroundTexture);
    UnloadTexture(missionarySeatedTexture);
    UnloadTexture(cannibalSeatedTexture);
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

void printPlayer(Player p, float scale) {
    Texture2D *texture;
    
    int isInBoat = 1;
    
    // Check with boat position
    isInBoat = (p.position.x >= boat.position.x && p.position.x <= boat.position.x + 250 &&
                p.position.y >= boat.position.y - 130 && p.position.y <= boat.position.y - 60);
    
    //changement de texture
    if (isInBoat) {
        texture = (p.type == 0) ? &missionarySeatedTexture : &cannibalSeatedTexture;
    } else {
        texture = (p.type == 0) ? &missionaryTexture : &cannibalTexture;
    }
    
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

void DrawSpeedSlider() {
    DrawText("Vitesse :", WIDTH - 300, HEIGHT - 70, 20, WHITE);
    
    if (speedSlider.width == 0) {
        speedSlider = (Rectangle){ WIDTH - 300, HEIGHT - 40, 200, 20 };
    }
    
    DrawRectangleRec(speedSlider, LIGHTGRAY);
    DrawRectangleLinesEx(speedSlider, 1, DARKGRAY);
    
    float knobX = speedSlider.x + (animationSpeedMultiplier - 0.5f) * (speedSlider.width / 39.5f);
    Rectangle knob = { knobX - 5, speedSlider.y - 5, 10, speedSlider.height + 10 };
    
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, knob) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        sliderActive = 1;
    }
    
    if (sliderActive) {
        if (IsMouseButtonUp(MOUSE_LEFT_BUTTON)) {
            sliderActive = 0;
        } else {
            float relativePos = (mousePos.x - speedSlider.x) / speedSlider.width;
            relativePos = Clamp(relativePos, 0.0f, 1.0f);
            animationSpeedMultiplier = 0.5f + relativePos * 39.5f; // RANGE 0.5 - 40
        }
    }
    
    DrawRectangleRec(knob, sliderActive ? RED : MAROON);
    
    char speedText[32];
    sprintf(speedText, "%.1fx", animationSpeedMultiplier);
    DrawText(speedText, WIDTH - 80, HEIGHT - 70, 20, WHITE);
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
    DrawText("Retour", backButton.x + backButton.width/2 - MeasureText("Retour", 20)/2,
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
    
             // Reset button
    Rectangle resetButton = { 260, 20, 100, 40 };
    Color resetButtonColor = LIGHTGRAY;
    
    if (CheckCollisionPointRec(mousePos, resetButton)) {
        resetButtonColor = GRAY;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            InitEntites();
            gamePaused = 0;
        }
    }
    
    DrawRectangleRec(resetButton, resetButtonColor);
    DrawRectangleLinesEx(resetButton, 2, BLACK);
    DrawText("Reset", resetButton.x + resetButton.width/2 - MeasureText("Reset", 20)/2,
             resetButton.y + resetButton.height/2 - 10, 20, BLACK);
    
    //  Slider Speed
    DrawSpeedSlider();

    char currentModeText[50];
    sprintf(currentModeText, "Mode: %s", current == STATE_PILE ? "PILE" : "FILE");
    DrawText(currentModeText, WIDTH - 250, 10, 20, WHITE);
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

    // Initialize start pile/file based on current mode
    if (current == STATE_PILE || current == STATE_INTRO) {
        // Initialize pile mode
        InitStartPile(players);
        PrintPileTypes(startPile);
        InitPile(&onBoat);
        InitPile(&endPile);
    } else {
        // Initialize file mode
        InitFileStart(players);
        PrintFileTypes(startFile);
        InitFile(&onBoatFile);
        InitFile(&endFile);
    }

    // Initialize boat
    boat = InitBoat();
    InitPile(&onBoat);
}

int movePlayers() {
    int moved = 0; 
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
                const float baseSpeed = 2.0f; 
                float speed = baseSpeed * animationSpeedMultiplier;
                
                if (speed >= distance) {
                    p->position = p->destination;
                    p->onMove = 0;
                } else {
                    // Normal
                    p->position.x += direction.x * speed;
                    p->position.y += direction.y * speed;
                }
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
            const float baseSpeed = 2.0f; 
            float speed = baseSpeed * animationSpeedMultiplier;
            
            if (speed >= distance) {
                boat.position = boat.destination;
                if(boat.destination.x == BOATSTARTX){
                    boat.location = 1;
                } else {
                    boat.location = 3;
                }
            } else {
                // Normal
                boat.position.x += direction.x * speed;
                boat.position.y += direction.y * speed;
            }
        }
    }
}

void FromStartToBoat(){
    if (current == STATE_PILE) {
        if(PileSize(startPile, 0) > 0){
            // Save the top element of startPile
            Pile *movedPile = malloc(sizeof(Pile));
            *movedPile = startPile;
            movedPile->prev = NULL;

            // pop element from stack
            if (startPile.prev == NULL) {
                InitPile(&startPile);
            } else {
                startPile = *startPile.prev;
            }

            // add the element to the boat
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
    } else if (current == STATE_FILE) {
        if(FileSize(startFile, 0) > 0) {
            File *movedFile = malloc(sizeof(File));
            *movedFile = startFile;
            movedFile->prev = NULL;

            printf("Deplacement du joueur de type: %d du debut au bateau\n", startFile.p->type);

            if (startFile.prev == NULL) {
                InitFile(&startFile);
            } else {
                startFile = *startFile.prev;
            }

            if (onBoatFile.p == NULL) {
                onBoatFile = *movedFile;
                free(movedFile);
            } else {
                File *current = &onBoatFile;
                while (current->prev != NULL) {
                    current = current->prev;
                }
                current->prev = movedFile;
            }
        }
    }
}
void FromBoatToEnd() {
    if (current == STATE_PILE) {
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
                // endPile empty
                endPile.p = movedPile->p;
                endPile.prev = movedPile->prev;
                free(movedPile);
            } else {
                // endPile isn't empty: add the element to the end
                Pile *oldTop = malloc(sizeof(Pile));
                *oldTop = endPile;
                endPile = *movedPile; 
                endPile.prev = oldTop;
                free(movedPile);
            }
        }
    } else if  (current == STATE_FILE) { 
        if(FileSize(onBoatFile, 0) > 0) {
            File *movedFile = malloc(sizeof(File));
            *movedFile = onBoatFile;
            movedFile->prev = NULL;

            if(onBoatFile.prev == NULL) {
                InitFile(&onBoatFile);
            } else {
                onBoatFile = *onBoatFile.prev;
            }

            if(endFile.p == NULL) {
                endFile = *movedFile;
                free(movedFile);
            } else {
                File *current = &endFile;
                while (current->prev != NULL) {
                    current = current->prev;
                }
                current->prev = movedFile;
            }
        }
    }
}
void SetDestinationToPlayerOnBoat(Vector2 destinationP1, Vector2 destinationP2){
    if (current == STATE_PILE) {
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
    } else if (current == STATE_FILE) {
        if(onBoatFile.p != NULL) {
            onBoatFile.p->destination = destinationP1;
            onBoatFile.p->onMove = 1;
            
            if (onBoatFile.prev != NULL && onBoatFile.prev->p != NULL) {
                onBoatFile.prev->p->destination = destinationP2;
                onBoatFile.prev->p->onMove = 1;
            }
        }
    }
}


void updatePlayersPositonsInBoat(){
    boatPosition[0].x = boat.position.x + 55;
    boatPosition[0].y = boat.position.y - 60;
    boatPosition[1].x = boat.position.x + 140;
    boatPosition[1].y = boat.position.y - 60;
}

void game() {
    if(boat.location == 0) {
        moveBoat();
        if(!movePlayers()) {
            printf("WARNING : Boat is moving without player(s)");
        }
    }
    else if(boat.location == 1) {
        if(!movePlayers()) {
            int boatSize = (current == STATE_PILE) ? PileSize(onBoat, 0) : FileSize(onBoatFile, 0);
            int startSize = (current == STATE_PILE) ? PileSize(startPile, 0) : FileSize(startFile, 0);
            
            if(boatSize == 2) {
                boat.location = 0;
                boat.destination.x = 400;
                boat.destination.y = 450;
                Vector2 destinationP1 = {boat.destination.x + 50, boat.position.y - 60};
                Vector2 destinationP2 = {boat.destination.x + 150, boat.position.y - 60};
                SetDestinationToPlayerOnBoat(destinationP2, destinationP1);
            }
            
            if(startSize == 6) {
                for(int i=0; i<2; i++) {
                    updatePlayersPositonsInBoat();
                    if(current == STATE_PILE) {
                        // For PILE: Always use the top element
                        startPile.p->onMove = 1;
                        startPile.p->destination = boatPosition[i];
                        FromStartToBoat();
                    } else {
                        // For FILE: Use each element in sequence and remove it
                        // Save the player pointer before removing from the queue
                        Player* playerToMove = startFile.p;
                        playerToMove->onMove = 1;
                        playerToMove->destination = boatPosition[i];
                        FromStartToBoat();
                    }
                }
            }
            
            if((startSize < 6) && (boatSize <= 1)) {
                updatePlayersPositonsInBoat();
                if(current == STATE_PILE) {
                    startPile.p->onMove = 1;
                    startPile.p->destination = boatPosition[1];
                    FromStartToBoat();
                } else if (startFile.p != NULL) {
                    // For FILE: Use each element in sequence
                    if(waitingPlayer){
                        Player* playerToMove = startFile.p;
                        playerToMove->onMove = 1;
                        playerToMove->destination = boatPosition[1];
                        FromStartToBoat();
                        File *toBoat = malloc(sizeof(File));
                        toBoat->p = waitingPlayer;
                        toBoat->prev = NULL;
                        toBoat->p->onMove = 1;
                        toBoat->p->destination = boatPosition[0];
                        onBoatFile.prev = toBoat;
                        waitingPlayer = NULL;
                    }
                    else{
                        BoatToWaiting(boatSize);
                    }

                }
            }
        }
    } else {
        if(!movePlayers()) {
            int boatSize = (current == STATE_PILE) ? PileSize(onBoat, 0) : FileSize(onBoatFile, 0);
            int startSize = (current == STATE_PILE) ? PileSize(startPile, 0) : FileSize(startFile, 0);
            int endSize = (current == STATE_PILE) ? PileSize(endPile, 0) : FileSize(endFile, 0);
            if(boatSize == 2) {
                Vector2 finalDestination;
                if(!endSize) {
                    finalDestination.x = 0;
                    finalDestination.y = 350;
                } else {
                    if (current == STATE_PILE) {
                        finalDestination = endPile.p->position;
                        if(endPile.p->type == 0) {
                            finalDestination.x += 110;
                        } else { 
                            finalDestination.x += 30;
                        }
                    } else {
                        finalDestination = GetLast(endFile).p->position;
                        if(endFile.p->type == 0) {
                            finalDestination.x += 80;
                        } else { 
                            finalDestination.x += 70;
                        }
                    }
                }
                if (current == STATE_PILE) {
                    Player* playerToMove = onBoat.p;
                    FromBoatToEnd();
                    endPile.p->onMove = 1;
                    endPile.p->destination = finalDestination;
                    printf("Deplacement du joueur de type: %d du bateau a la fin\n", endPile.p->type);
                } else {
                    Player* lastPlayer = onBoatFile.p;
                    FromBoatToEnd();
                    File lastElement;
                    lastElement = GetLast(endFile);
                    lastElement.p->onMove = 1;
                    lastElement.p->destination = finalDestination;
                }
                
                if(!startSize) {
                    finalDestination.x = 150;
                    finalDestination.y = 550;
                    
                    if (current == STATE_PILE) {
                        Player* lastPlayer = onBoat.p;
                        FromBoatToEnd();
                        endPile.p->onMove = 1;
                        endPile.p->destination = finalDestination;
                        printf("Deplacement du joueur de type: %d du bateau a la fin\n", endPile.p->type);
                    } else {
                        Player* lastPlayer = onBoatFile.p;
                        FromBoatToEnd();
                        File lastElement;
                        lastElement = GetLast(endFile);
                        lastElement.p->onMove = 1;
                        lastElement.p->destination = finalDestination;
                    }
                }
            } else if(boatSize == 1) {
                printf("retour lobby\n");
                if ((current == STATE_PILE && startPile.p) || 
                    (current == STATE_FILE && startFile.p)) {
                    boat.location = 0;
                    boat.destination.x = BOATSTARTX;
                    boat.destination.y = 450;
                    
                    if (current == STATE_PILE) {
                        onBoat.p->onMove = 1;
                        Vector2 destination = {boat.destination.x + 100, boat.position.y - 60};
                        onBoat.p->destination = destination;
                    } else {
                        onBoatFile.p->onMove = 1;
                        Vector2 destination = {boat.destination.x + 100, boat.position.y - 60};
                        onBoatFile.p->destination = destination;
                    }
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

    GameState previousState = STATE_INTRO;

    while (!WindowShouldClose()){
        BeginDrawing();
        float scale = (float)(WIDTH) / (float)(backgroundTexture.width);
        float posX = (WIDTH - (backgroundTexture.width * scale)) * 0.5f;
        float posY = (HEIGHT - (backgroundTexture.height * scale)) * 0.5f;

        if (previousState != current) {
            InitEntites();
            previousState = current;
        }

        switch(current) {
            case STATE_INTRO:
                ClearBackground(BLACK);
                DrawIntro();
                break;
            case STATE_PILE:
            case STATE_FILE:
                ClearBackground(BLACK);
                DrawTextureEx(backgroundTexture, (Vector2){posX, posY}, 0.0f, scale, WHITE);
                PrintEntities();

                if (!gamePaused) {
                    game();
                }
                
                DrawGameControls();
                break;
        }
        EndDrawing();
    }

    UnloadGameTextures();
    CloseWindow();
    return 0;
}