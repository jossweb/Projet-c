#ifndef FILE_H
#define FILE_H

typedef struct Player Player;

struct File {
    Player *p;
    struct File *prev;
};
typedef struct File File;

File startFile;
File onBoatFile;
File endFile;

int FileSize(File f, int size);
void InitFile(File *f);
Player GetLastPlayerInPile(File f);
void InitFileStart(Player *players[]);
void PrintFileTypes(File f);
#endif