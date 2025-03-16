#ifndef FILE_H
#define FILE_H
#include "raylib.h"

typedef struct Player Player;

struct File {
    Player *p;
    struct File *prev;
};
typedef struct File File;

File startFile;
File onBoatFile;
File endFile;
Player *waitingPlayer;
extern const Vector2 WAITINGPOSITION;

int FileSize(File f, int size);
void InitFile(File *f);
Player GetLastPlayerInPile(File f);
void InitFileStart(Player *players[]);
void PrintFileTypes(File f);
void BoatToWaiting(int boatSize);
File GetLast(File f);
#endif