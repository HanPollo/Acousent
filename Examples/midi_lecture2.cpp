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

void ProcessMIDIAndRender(const ALuint* midiData, ALuint* audioStream, int streamLength)
{
    int sampleBlock, sampleCount = streamLength / (2 * sizeof(float)); // 2 output channels

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

        // Move the audio stream pointer forward by the number of processed samples
        audioStream += sampleBlock * (2 * sizeof(float));
    }
}



int main(int argc, char* argv[])
{
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
    ALCint attributes[] = { ALC_FREQUENCY, 44100, 0 };
    alcMakeContextCurrent(context);

    // Generate an OpenAL buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // Generate an OpenAL source
    ALuint source;
    alGenSources(1, &source);

    // Load the SoundFont from a file
    g_TinySoundFont = tsf_load_filename(ac::getPath("Resources/Audio/SoundFonts/florestan-subset.sf2").string().c_str());
    if (!g_TinySoundFont)
    {
        fprintf(stderr, "Could not load SoundFont\n");
        return 1;
    }

    // Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
    tsf_channel_set_bank_preset(g_TinySoundFont, 9, 128, 0);

    // Set the SoundFont rendering output mode
    tsf_set_output(g_TinySoundFont, TSF_STEREO_INTERLEAVED, 44100, 0.0f);

    // Load the MIDI file
    TinyMidiLoader = tml_load_filename(ac::getPath("Resources/Audio/MIDI/venture.mid").string().c_str());
    if (!TinyMidiLoader)
    {
        fprintf(stderr, "Could not load MIDI file\n");
        return 1;
    }

    // Set up the global MidiMessage pointer to the first MIDI message
    g_MidiMessage = TinyMidiLoader;

    // Start the actual audio playback here
    alSourcePlay(source);
    ALint buffersQueued;

    // Process and play the MIDI messages
    while (g_MidiMessage != nullptr)
    {
        // Calculate the number of samples to render
        int sampleCount = 4096;
        int sampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK;
        if (sampleCount < sampleBlock)
            sampleBlock = sampleCount;

        // Dynamically allocate the samples array
        float* samples = new float[sampleBlock * 2];

        // Render the audio samples
        tsf_render_float(g_TinySoundFont, samples, sampleBlock, 0);

        // Set the buffer data
        alBufferData(buffer, AL_FORMAT_STEREO_FLOAT32, samples, sampleBlock * sizeof(float) * 2, 44100);

        // Queue the buffer to the source
        alSourceQueueBuffers(source, 1, &buffer);

        // Check if the source is playing or has stopped
        ALint sourceState;
        alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
        if (sourceState != AL_PLAYING)
        {
            // Start the source if it's not playing
            alSourcePlay(source);
            cout << "Play ";
        }

        // Wait for all buffers to be processed
        ALint buffersQueued;
        ALint buffersProcessed;
        do
        {
            // Check the number of buffers queued and processed
            alGetSourcei(source, AL_BUFFERS_QUEUED, &buffersQueued);
            alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffersProcessed);

            // Wait for a short period or use an appropriate sleep or delay function
            Sleep(10);

        } while (buffersQueued > 0 && buffersProcessed < buffersQueued);

        // Unqueue all processed buffers
        while (buffersProcessed > 0)
        {
            alSourceUnqueueBuffers(source, 1, &buffer);
            buffersProcessed--;
        }

        cout << "All buffers processed" << endl;


        // Update the MIDI playback time and move to the next MIDI message
        g_Msec += sampleBlock * (1000.0 / 44100.0);
        while (g_MidiMessage && g_Msec >= g_MidiMessage->time)
        {
            // Process the MIDI message
            switch (g_MidiMessage->type)
            {
            case TML_PROGRAM_CHANGE: // channel program (preset) change (special handling for 10th MIDI channel with drums)
                tsf_channel_set_presetnumber(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->program, (g_MidiMessage->channel == 9));
                break;
            case TML_NOTE_ON: // play a note
                tsf_channel_note_on(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key, g_MidiMessage->velocity / 127.0f);
                break;
            case TML_NOTE_OFF: // stop a note
                tsf_channel_note_off(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key);
                break;
            case TML_PITCH_BEND: // pitch wheel modification
                tsf_channel_set_pitchwheel(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->pitch_bend);
                break;
            case TML_CONTROL_CHANGE: // MIDI controller messages
                tsf_channel_midi_control(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->control, g_MidiMessage->control_value);
                break;
            }

            // Move to the next MIDI message
            g_MidiMessage = g_MidiMessage->next;
        }

        // Delete the dynamically allocated samples array
        delete[] samples;
    }
    cout << "Salio del while loop" << endl;
    // Stop the audio playback
    alSourceStop(source);

    // Delete the OpenAL buffer and source
    alDeleteBuffers(1, &buffer);
    alDeleteSources(1, &source);

    // Clean up resources
    tsf_close(g_TinySoundFont);
    tml_free(TinyMidiLoader);

    // Destroy the OpenAL context and device
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}
