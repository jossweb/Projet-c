![Problème des missionnaires et des cannibales](assets/banner.svg)

## Télécharger raylib sous MacOS(Intel/Apple silicon)
prérequis : avoir brew d'installé

```bash
brew install raylib
```

## Télécharger et build raylib sous linux

prérequis : /
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
cc main.c pile.c file.c common.c -o main $(pkg-config --libs --cflags raylib)
./main
```

# et sous windows 

 C:/msys64/mingw64/bin/gcc.exe -fdiagnostics-color=always -g main.c file.c pile.c file.h pile.h common.c common.h -o main.exe -I C:/msys64/mingw64/include -L C:/msys64/mingw64/lib -lraylib -lopengl32 -lgdi32 -lwinmm -fcommon

 (je suis oblige de mettre le .h sinon le compilateur ne compile pas)