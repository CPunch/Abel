# Building the Abel game engine

The following libraries and tools are required:
- CMake (>=3.16)
- SDL2
- SDL2_image
- SDL2_ttf
- SDL2_audio

## Linux and other unix-like OSes

Simply generate the Abel makefiles using CMake:

```sh
$ cmake -B build
```
> If you're missing any libraries, CMake will complain!

Now compile the target:

```sh
$ cmake --build build
```
