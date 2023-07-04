#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#define TSF_IMPLEMENTATION
#include "../third_party/tsf/tsf.h"
#define TML_IMPLEMENTATION
#include "../third_party/tsf/tml.h"

#include "root_directory.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <iostream>

namespace ac = Acousent;
using namespace std;

// Function to initialize OpenAL and create an audio context
ALCdevice* InitOpenAL()
{
    ALCdevice* device = alcOpenDevice(NULL);
    ALCcontext* context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
    return device;
}

// Function to load the SoundFont file using TinySoundFont
tsf* LoadSoundFont(const char* soundfontPath)
{
    tsf* soundfont = tsf_load_filename(soundfontPath);
    if (!soundfont) {
        std::cout << "Failed to load SoundFont." << std::endl;
        exit(1);
    }
    return soundfont;
}

// Function to convert floating-point samples to OpenAL format
std::vector<ALshort> ConvertToOpenALFormat(const float* samples, size_t numSamples)
{
    std::vector<ALshort> convertedSamples(numSamples);
    for (size_t i = 0; i < numSamples; ++i) {
        convertedSamples[i] = static_cast<ALshort>(samples[i] * 32767.0f);
    }
    return convertedSamples;
}


// Holds the global instance pointer
static tsf* soundfont;

int main()
{
    // Initialize OpenAL
    ALCdevice* device = InitOpenAL();

    // Load the SoundFont
    soundfont = LoadSoundFont(ac::getPath("Resources/Audio/SoundFonts/florestan-subset.sf2").string().c_str());

    // Load the MIDI file using TinyMML
    tml_message* midiFile = tml_load_filename(ac::getPath("Resources/Audio/MIDI/venture.mid").string().c_str());
    if (!midiFile) {
        std::cout << "Failed to load MIDI file." << std::endl;
        exit(1);
    }

    // Set up rendering options
    tsf_set_output(soundfont, TSF_STEREO_INTERLEAVED, 44100, 0.0f);

    // Render the MIDI track with the SoundFont
    std::vector<float> audioBuffer(44100); // Adjust the buffer size as needed
    int channels;
    int programs;
    int total_notes;
    unsigned int first_note;
    unsigned int time_length;
    tml_get_info(midiFile, &channels, &programs, &total_notes, &first_note, &time_length);

    float durationSeconds = static_cast<float>(time_length) / 1000.0f;
    size_t numSamples = static_cast<size_t>(durationSeconds * 44100.0f);

    tsf_render_float(soundfont, audioBuffer.data(), static_cast<int>(numSamples), 0);

    // Convert samples to OpenAL format
    std::vector<ALshort> openALBuffer = ConvertToOpenALFormat(audioBuffer.data(), time_length);

    // Generate and play an OpenAL buffer
    ALuint bufferID;
    alGenBuffers(1, &bufferID);
    alBufferData(bufferID, AL_FORMAT_MONO16, openALBuffer.data(), static_cast<ALsizei>(openALBuffer.size() * sizeof(ALshort)), 44100);

    ALuint sourceID;
    alGenSources(1, &sourceID);
    alSourcei(sourceID, AL_BUFFER, bufferID);
    alSourcePlay(sourceID);

    // Wait until playback is finished
    ALint state;
    do {
        alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    // Clean up resources
    alDeleteSources(1, &sourceID);
    alDeleteBuffers(1, &bufferID);
    tml_free(midiFile);
    tsf_close(soundfont);
    alcDestroyContext(alcGetCurrentContext());
    alcMakeContextCurrent(NULL);
    
    alcCloseDevice(device);
    cout << "Finished" << endl;
    return 0;
}


