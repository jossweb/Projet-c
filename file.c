#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "raylib.h" 
#include "common.h"

const Vector2 WAITINGPOSITION = {1000, 500};

int FileSize(File f, int size){
    if(!f.p ){
        return size;
    }
    if(!f.prev) {
        return ++size;
    }
    return FileSize(*f.prev, ++size);
}
void InitFile(File *f){
    f->p = NULL;
    f->prev = NULL;
}
Player GetLastPlayerInPile(File f){
    if(!f.prev){
        return *f.p;
    }
    return GetLastPlayerInPile(*f.prev);
}
void InitFileStart(Player *players[]) {
    startFile.p = players[0];
    startFile.prev = NULL;
    File *lastElement = &startFile;
    for (int i = 1; i < 6; i++) {
        lastElement->prev = malloc(sizeof(File));
        lastElement = lastElement->prev;
        lastElement->p = players[6-i];
        lastElement->prev = NULL;
    }
}
void PrintFileTypes(File f) {
    if(!f.p) {
        printf("File is empty\n");
        return;
    }
    printf("Player type: %d\n", f.p->type);
    
    if(f.prev != NULL) {
        PrintFileTypes(*f.prev);
    } else {
        printf("Fin de la File\n");
    }
}
void BoatToWaiting(int boatSize){
    if(boatSize){
        onBoatFile.p->onMove = 1;
        onBoatFile.p->destination = WAITINGPOSITION;
        waitingPlayer = onBoatFile.p;
        onBoatFile.p = NULL;
    }
}
File GetLast(File f){
    if(!f.prev){
        return f;
    }
    return GetLast(*f.prev);
}