# Acousent
Orchestration Software for 3D Audio developed in C++ for the graduation process in the Computer Engineering program at the University of Chile.

## Thesis Document
The written document that details the work carried out in this thesis is in the `Memoria` folder. 

## Dependencies
Acousent has a list of different external libraries which are in the `third_Party/`directory.

 - [GLFW](https://www.glfw.org/) window and input managing.
 - [glad](https://glad.dav1d.de/) to use OpenGL functions. 
 - [glm](https://glm.g-truc.net/0.9.9/index.html) for linear algebra.
 - [dr_libs](https://github.com/mackron/dr_libs) for .wav and other audio file parsing.
 - [OpenAL-Soft](https://github.com/kcat/openal-soft) for audio implementation and playback.
 - [Assimp](https://github.com/assimp/assimp) for model parsing and loading.
 - [nlohmann/json](https://github.com/nlohmann/json) for json parsing and managing.
 - [TinySoundFont](https://github.com/schellingb/TinySoundFont) for midi and soundFont parsing and loading.
 - [stb](https://github.com/nothings/stb) to load images from disc.
 - [RtMidi](https://github.com/thestk/rtmidi) for midi input lecture from midi controller hardware.
 
## Software/Libreries needed
To generate the project, solution or makefiles [CMake 3.5+](https://cmake.org/), OpenGL and [Git 2.13+](https://git-scm.com/) are needed.

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
Not supported yet.

## How to use program
To utilize the software, the central file is "distribution.json" located in `Acousent/Resources/Config/distribution.json`. Sample and example files are also provided. The main file lists 2 speakers and 5 instruments in specific positions, which users can modify. Before executing the main program, users can reconfigure the placement instruments and speakers. Audio files should be correctly placed based on their formats: WAVE files in `Resources/Audio/Wav`, MIDI files in `Resources/Audio/MIDI`, and SoundFont files in `Resources/Audio/SoundFonts`.

Upon running the main program, the software console displays the speakers and instruments with their respective audio files and positions. In the main window, a room with the instruments on a stage is presented. Users can navigate using the WASD keys and adjust the camera using the mouse. Sound playback can be started and paused with the space key, looped with the 'L' key, and the program can be exited with the escape key. Camera rotation and movement are essential for accessing all parts of the room.

It's vital for audio files to have similar durations to ensure synchronous looping. If files get out of sync, looping should be deactivated, and all audios should finish to resynchronize. Ideally, the software should use a MIDI file divided into various tracks corresponding to different ensemble instruments. If a user is uncertain about the tracks or presets for a MIDI file with a soundfont, they can run the demo "files_info.exe" in the `Examples` folder and input the MIDI and SoundFont filenames.

The demo MIDI file "venture.mid" and its associated SoundFont were taken from TinySoundFont samples. The other audio resources were developed by the thesis author. The 3D Models used as demo have their respective licenses in their folders.

Have fun listening to 3D audio and have high hopes for the future of this project.
