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

// Holds the global instance pointer
static tsf* g_TinySoundFont;

// Holds global MIDI playback state
static double g_Msec;               //current playback time
static tml_message* g_MidiMessage;  //next message to be played

int test_int = 0;


void ProcessMIDIAndRender(ALuint* audioStream, int streamLength)
{
    int sampleBlock, sampleCount = streamLength / (1 * sizeof(float)); // 1 output channel

    for (sampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK; sampleCount > 0; sampleCount -= sampleBlock)
    {
        // Progress the MIDI playback and process a block of samples
        if (sampleBlock > sampleCount)
            sampleBlock = sampleCount;

        // Process MIDI messages up to the current playback time
        for (g_Msec += sampleBlock * (1000.0 / 44100.0); g_MidiMessage && g_Msec >= g_MidiMessage->time; g_MidiMessage = g_MidiMessage->next)
        {
            switch (g_MidiMessage->type)
            {
            case TML_PROGRAM_CHANGE:
                tsf_channel_set_presetnumber(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->program, (g_MidiMessage->channel == 9));
                break;
            case TML_NOTE_ON:
                tsf_channel_note_on(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key, g_MidiMessage->velocity / 127.0f);
                break;
            case TML_NOTE_OFF:
                tsf_channel_note_off(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key);
                break;
            case TML_PITCH_BEND:
                tsf_channel_set_pitchwheel(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->pitch_bend);
                break;
            case TML_CONTROL_CHANGE:
                tsf_channel_midi_control(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->control, g_MidiMessage->control_value);
                break;
            }
        }

        // Render the block of audio samples in float format
        tsf_render_float(g_TinySoundFont, (float*)audioStream, sampleBlock, 0);
        test_int++;
        if (test_int > 2267) {
            //cout << "Test_int es maypr a 2267" << endl;
            //auto float_size = sizeof(float);
            //cout << "Float size: " << float_size << endl;
        }
        // Calculate the number of bytes processed in the block
        int blockSizeBytes = sampleBlock * (1 * sizeof(float));

        // Increment the audio stream pointer by the number of samples processed
        audioStream += blockSizeBytes / sizeof(unsigned int);
        //audioStream += 1;

    }
}

int CalculateStreamLength(double sampleRate)
{
    int maxTimestamp = 0;

    // Iterate through the MIDI messages and find the maximum timestamp
    for (tml_message* message = g_MidiMessage; message != NULL; message = message->next)
    {
        if (message->time > maxTimestamp)
        {
            maxTimestamp = message->time;
        }
    }

    // Calculate the stream length based on the maximum timestamp and sample rate (Divided by 1000 to convert to seconds)
    int streamLength = (int)(maxTimestamp * sampleRate / 1000.0) * (1 * sizeof(float)); // 1 Channel

    return streamLength;
}



int main(int argc, char* argv[])
{
    const auto sampleRate = 44100.0;
    tml_message* TinyMidiLoader = nullptr;

    // Initialize the OpenAL context and device
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device)
    {
        fprintf(stderr, "Could not open OpenAL device\n");
        return 1;
    }
    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context)
    {
        fprintf(stderr, "Could not create OpenAL context\n");
        return 1;
    }
    // Set the desired sample rate
    ALCint attributes[] = { ALC_FREQUENCY, sampleRate, 0 };
    alcMakeContextCurrent(context);

    // Generate an OpenAL buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // Generate an OpenAL source
    ALuint source;
    alGenSources(1, &source);

    // Load the SoundFont from a file
    g_TinySoundFont = tsf_load_filename(ac::getPath("Resources/Audio/SoundFonts/ElectricPiano.sf2").string().c_str());
    if (!g_TinySoundFont)
    {
        fprintf(stderr, "Could not load SoundFont\n");
        return 1;
    }

    // Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
    tsf_channel_set_bank_preset(g_TinySoundFont, 9, 128, 0);

    // Set the SoundFont rendering output mode
    tsf_set_output(g_TinySoundFont, TSF_MONO, sampleRate, 0.0f);

    // Load the MIDI file
    TinyMidiLoader = tml_load_filename(ac::getPath("Resources/Audio/MIDI/venture.mid").string().c_str());
    if (!TinyMidiLoader)
    {
        fprintf(stderr, "Could not load MIDI file\n");
        return 1;
    }

    // Set up the global MidiMessage pointer to the first MIDI message
    g_MidiMessage = TinyMidiLoader;

    auto streamLength = CalculateStreamLength(sampleRate);
    // Allocate memory for the audio stream
    ALuint* audioStream = new ALuint[streamLength / sizeof(ALuint)];

    // Call the ProcessMIDIAndRender function
    ProcessMIDIAndRender(reinterpret_cast<ALuint*>(audioStream), streamLength);

    cout << "Iteraciones: " << test_int << endl;
    auto tamano = streamLength / sizeof(ALuint);
    cout << "Tamano audioStream: " << tamano << endl;
    cout << "streamLength: " << streamLength << endl;


    ALuint bufferID;
    alGenBuffers(1, &bufferID);
    alBufferData(bufferID, AL_FORMAT_MONO_FLOAT32, audioStream, static_cast<ALsizei>(streamLength), sampleRate);

    ALuint sourceID;
    alGenSources(1, &sourceID);
    alSourcei(sourceID, AL_BUFFER, bufferID);
    //alSourcei(sourceID, AL_LOOPING, AL_TRUE);
    alSourcePlay(sourceID);

    ALint sourceState;
    alGetSourcei(sourceID, AL_SOURCE_STATE, &sourceState);
    while (true) {
        alGetSourcei(sourceID, AL_SOURCE_STATE, &sourceState);
        if (sourceState != AL_PLAYING) {
            // Playback has completed
            break;
        }
        // You can add a sleep or delay here to avoid busy waiting
    }


    // Delete the OpenAL buffer and source
    alDeleteBuffers(1, &buffer);
    alDeleteSources(1, &source);

    alDeleteBuffers(1, &bufferID);
    alDeleteSources(1, &sourceID);

    // Clean up resources
    delete[] audioStream;
    tsf_close(g_TinySoundFont);
    tml_free(TinyMidiLoader);

    // Destroy the OpenAL context and device
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}
