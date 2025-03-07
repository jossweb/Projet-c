#include <stdio.h>
#include <stdlib.h>
#include "pile.h"
#include "pile.h"
#include "raylib.h" 
#include "common.h"


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