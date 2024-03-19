# Ice Cavern Game Engine

## Setup

1. Install and setup [CMake](https://cmake.org/download/).
2. Install and setup [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd#1---set-up-vcpkg)
    - Follow steps 1 and 2 to clone the repo, run the bootstrap script, and setup env variables.
3. Run CMake Configuration

```sh
cmake --preset=default
```

4. Build the project

```sh
cmake --build build
```

5. Run the executable

```sh
./build/IceCavernGameEngine
```
