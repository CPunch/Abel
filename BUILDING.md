# Building the Abel game engine

The following libraries and tools are required:
- CMake (>=3.16)
- SDL2
- SDL2_image
- SDL2_ttf
- SDL2_audio

## Windows

I personally use Visual Studio 2022 && exclusively the developer command prompt. However I don't use Windows as my main development environment and only periodically use a vm to verify builds.

Make sure to [install vcpkg](https://vcpkg.io/en/getting-started.html), and then install the relevant libraries while in the root repo directory with:
```batch
vcpkg install --triplet x64-windows
```
> Make sure you've integrated vcpkg using `vcpkg integrate install` beforehand

Then, generate a Visual Studio .sln with:
```batch
cmake -B build -A x64 -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
```
> Be sure to set VCPKG_ROOT in your environment variables if you haven't

Finally, you should be able to compile the project normally with:
```batch
cmake --build build --config Release
```

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
