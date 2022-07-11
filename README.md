# Acousent
Orchestration Software for 3D Audio developed in C++.
## Dependencies
Acousent has a list of different external libraries which are in the `third_Party/`directory.

 - [GLFW](https://www.glfw.org/) window and input managing.
 - [glad](https://glad.dav1d.de/) to use OpenGL functions. 
 - [glm](https://glm.g-truc.net/0.9.9/index.html) for linear algebra.
 - [dr_libs](https://github.com/mackron/dr_libs) for .wav and other audio file parsing.
 - [OpenAL-Soft](https://github.com/kcat/openal-soft) for audio implementation.
 - [MainLoop](https://github.com/mattearly/MainLoop) for software loop managing.
 - [EnTT](https://github.com/skypjack/entt) for ecs implementation and managing (not used in this iteration).
 
## Software/Libreries needed
To generate the project, solution or makefiles [CMake 3.15+](https://cmake.org/), OpenGL and [Git 2.13+](https://git-scm.com/) are needed.

## Building project
### Windows
To build the project first open the desired directory and use any git suportive terminal to clone the repository with dependencies.
```
git clone --recursive https://github.com/HanPollo/Acousent.git
```
Once cloned check if the dependencies where cloned into the different directories in `third_Party/`. They should all have folders and files inside.

If all or any folders in `third_Party/` are empty then try the folowing git command in the main directory (`Acousent/`)
```
git submodule update --init --recursive
```
If any folders in `third_Party/` are empty then check your SSH github keys and make sure you are correctly logged in to gihub and try the previous step again.

If everything cloned correctly open the `Acousent/` directory with Visual Studio (or any terminal with cmake support).
Open a terminal with the `Acousent/` directory (preferably Visual Studio's Developer PowerShell) and run the following code.

```
mkdir build
cd build
cmake ..
cmake --build .  //Or alternatevily Open Acousent/build/Acousent.sln with Visual Studio and build all solutions
```
After this just go to `Acousent/build/source/Debug/` and run `main.exe` for demo.
Or in same terminal (`Acousent/build/`)
```
./source/Debug/main.exe
```

### Linux and MacOS
Software does not work in these operating systems yet.

## How to use demo
To use the demo, once opened a window should appear with several green squares and a big beige rectangle with two red squares at the top.
The beige represents the stage, the green squares the audience and the red squares the audio sources. Preferably with headphones you will listen to a constant audio from the right-most source and with the `WASD` keys you can change from seat to seat in the audience. If the `Spacebar` is pressed the left-most source will begin playing the same audio but from a different location (where the left-most source is). With the `Escape` key you can exit the program.

Have fun listening to 3D audio and have high hopes for the future of this project.
