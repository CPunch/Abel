# Building Abel

The following libraries and tools are required:
- CMake (>=3.16)
- SDL2
- SDL2_image
- SDL2_ttf
- SDL2_audio

## Cross-platform: Using vcpkg

[vcpkg](https://vcpkg.io/en/) will take care of downloading and installing it's own copies of the required libraries.
If you don't care for the slight bloat, you can `git pull` the `vendor/vcpkg` submodule and use the `vcpkg` preset instead.
```bash
cmake --preset vcpkg
```

and compile the target with:
```
cmake --build --preset release
```

You can even run the testsuite:
```bash
ctest --preset test-release
```
