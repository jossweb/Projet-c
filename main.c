//  _____   __                       __       _                           
// |  __ \ /_/                      / /      | |                          
// | |__) |___ _ __ ___  _   _     / /       | | ___  ___ ___ _   _  __ _ 
// |  _  // _ | '_ ` _ \| | | |   / /    _   | |/ _ \/ __/ __| | | |/ _` |
// | | \ |  __| | | | | | |_| |  / /    | |__| | (_) \__ \__ | |_| | (_| |
// |_|  \_\___|_| |_| |_|\__, | /_/      \____/ \___/|___|___/\__,_|\__,_|
//                        __/ |                                           
//                       |___/                                            

#include "raylib.h"

#define WIDTH 1400
#define HEIGHT 850

struct Player{
    Vector2 position;
    int type; // 0 = missionary; 1 = cannibal
};

typedef struct Player Player;

struct boat
{
    Vector2 position;
    float speed;
    Color color;
};
typedef struct boat boat;

struct PlayerInPile
{
    Player p; 
    Player *nextP;
};

typedef struct PlayerInPile PlayerInPile;

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Projet C par Rémy.M et Jossua.F");

    Texture2D backgroundTexture = LoadTexture("assets/fond.png");
    
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(BLACK);
            
            float scale = (float)(WIDTH) / (float)(backgroundTexture.width);
            float posX = (WIDTH - (backgroundTexture.width * scale)) * 0.5f;
            float posY = (HEIGHT - (backgroundTexture.height * scale)) * 0.5f;
            
            DrawTextureEx(backgroundTexture, (Vector2){posX, posY}, 0.0f, scale, WHITE);
        EndDrawing();
    }

    UnloadTexture(backgroundTexture);
    
    CloseWindow();
    return 0;
}
Player createplayer(int type){
    Player p;
    p.position.x = 0;
    p.position.y = 0;
    p.type = type;
    return p;
}
void printPlayer(Player p){
    Texture2D playerTexture;
    if(p.type == 0){
        playerTexture = LoadTexture("assets/missionnaire.png");
    }else{ 
        playerTexture = LoadTexture("assets/cannibale.png");
    }
    DrawTexture(playerTexture, p.position.x, p.position.y, WHITE);
    UnloadTexture(playerTexture);
}