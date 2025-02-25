//
//  main.c
//  ProjetC
//
//  Created by Jossua Figueiras AND Rémy Maubert on 24/02/2025.
//
#include "raylib.h"

#define WIDTH 800
#define HEIGHT 450

struct Player{
    Vector2 position;
    int type; // 0 = missionary; 1 = cannibals
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
typedef struct pile *PlayerInPile;

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Projet C par Rémy.M et Jossua.F");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
void printPlayer(Player p){
    
}
