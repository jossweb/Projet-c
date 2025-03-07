#ifndef PILE_H
#define PILE_H

typedef struct Player Player;

struct Pile {
    Player *p;
    struct Pile *prev;
};
typedef struct Pile Pile;

//global piles
Pile startPile;
Pile onBoat;
Pile endPile;

int PileSize(Pile p, int size);
void InitPile(Pile *p);
void PrintPileTypes(Pile p);
void InitStartPile(Player *players[]);

#endif