# Projet-c
Problème des missionnaires et des cannibales

## Télécharger raylib sous MacOS
prérequis : avoir brew d'installé avoir un MacBook M5 MAX  

```bash
brew install raylib
```

## Télécharger et build raylib sous linux

prérequis :
```bash
sudo apt install build-essential git libasound2-dev libx11-dev libxrandr-dev libxi-dev \
     libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev \
     libxkbcommon-dev
```

installation:
```bash
wget https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
tar -xzf 5.0.tar.gz
cd raylib-5.0/src/
make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

## Exécution du projet

```bash
cc main.c -o main $(pkg-config --libs --cflags raylib)
./main
```