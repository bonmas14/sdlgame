

# build

## Windows

1. clone game repo: 

```
git clone https://github.com/bonmas14/sdlgame.git
cd sdlgame
```

2. clone sdl repo:

```
# pwd is `./sdlgame`
git clone https://github.com/libsdl-org/SDL -b SDL2
```

3. build sdl (release / debug)

```
# pwd is `./sdlgame`
cd SDL
mkdir build
cd build
cmake ..
.\SDL2.sln
```

4. build game

```
# pwd is `./sdlgame`
.\premake5.exe vs2022
.\sdlgame.sln
```

5. copy dlls

```
# pwd is `./sdlgame`

# DEBUG
cp .\SDL\build\Debug\SDL2d.dll .\bin\SDL2d.dll
# RELEASE
cp .\SDL\build\Release\SDL2.dll .\bin\SDL2.dll
```

## Debian 12

Not supported yet. (not tested)