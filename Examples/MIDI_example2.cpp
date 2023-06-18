#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#define TSF_IMPLEMENTATION
#include "../third_party/tsf/tsf.h"
#include "root_directory.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace ac = Acousent;
// ac::getPath("Resources/Audio/SoundFonts/florestan-subset.sf2").string().c_str()



// Holds the global instance pointer
static tsf* g_TinySoundFont;

// A Mutex so we don't call note_on/note_off while rendering audio samples
static ALCcontext* g_Context;
static ALCdevice* g_Device;

int main(int argc, char* argv[])
{
    int i, Notes[7] = { 48, 50, 52, 53, 55, 57, 59 };

    // Initialize the OpenAL context and device
    g_Device = alcOpenDevice(NULL);
    if (!g_Device)
    {
        fprintf(stderr, "Could not open OpenAL device\n");
        return 1;
    }
    g_Context = alcCreateContext(g_Device, NULL);
    if (!g_Context)
    {
        fprintf(stderr, "Could not create OpenAL context\n");
        return 1;
    }
    alcMakeContextCurrent(g_Context);

    // Load the SoundFont from a file
    g_TinySoundFont = tsf_load_filename(ac::getPath("Resources/Audio/SoundFonts/florestan-subset.sf2").string().c_str());
    if (!g_TinySoundFont)
    {
        fprintf(stderr, "Could not load SoundFont\n");
        return 1;
    }

    // Set the SoundFont rendering output mode
    tsf_set_output(g_TinySoundFont, TSF_STEREO_INTERLEAVED, 44100, 0);

    // Generate OpenAL buffers
    const int numBuffers = 2;
    ALuint buffers[numBuffers];
    alGenBuffers(numBuffers, buffers);

    // Generate an OpenAL source
    ALuint source;
    alGenSources(1, &source);

    // Initialize the buffers with silence
    float silence[4096] = { 0.0f };
    for (int i = 0; i < numBuffers; ++i)
    {
        alBufferData(buffers[i], AL_FORMAT_STEREO_FLOAT32, silence, sizeof(silence), 44100);
    }

    // Queue the buffers to the source
    alSourceQueueBuffers(source, numBuffers, buffers);

    // Start the actual audio playback here
    alSourcePlay(source);

    // Loop through all the presets in the loaded SoundFont
    for (i = 0; i < tsf_get_presetcount(g_TinySoundFont); i++)
    {
        // End the previous note and play a new note
        printf("Play note %d with preset #%d '%s'\n", Notes[i % 7], i, tsf_get_presetname(g_TinySoundFont, i));
        tsf_note_off(g_TinySoundFont, i - 1, Notes[(i - 1) % 7]);
        tsf_note_on(g_TinySoundFont, i, Notes[i % 7], 1.0f);

        // Wait until a buffer is no longer in use
        ALint numProcessed;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &numProcessed);
        while (numProcessed <= 0)
        {
            alGetSourcei(source, AL_BUFFERS_PROCESSED, &numProcessed);
#ifdef _WIN32
            Sleep(1000); // Windows: Sleep for 1 millisecond
#else
            usleep(1000); // Linux: Sleep for 1000 microseconds (1 millisecond)
#endif
        }

        // Unqueue the processed buffer
        ALuint processedBuffer;
        alSourceUnqueueBuffers(source, 1, &processedBuffer);

        // Render the audio samples into the processed buffer
        float buffer[4096];
        tsf_render_float(g_TinySoundFont, buffer, 4096 / 2, 0);

        // Set the buffer data
        alBufferData(processedBuffer, AL_FORMAT_STEREO_FLOAT32, buffer, sizeof(float) * 4096, 44100);

        // Queue the updated buffer back to the source
        alSourceQueueBuffers(source, 1, &processedBuffer);

        // Resume playing the source if it was stopped
        ALint sourceState;
        alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
        if (sourceState != AL_PLAYING)
        {
            alSourcePlay(source);
        }

        // Delay for the desired note duration
#ifdef _WIN32
        Sleep(2000); // Windows: Sleep for 2000 milliseconds (2 seconds)
#else
        usleep(2000000); // Linux: Sleep for 2000000 microseconds (2 seconds)
#endif
    }

    // Cleanup resources
    tsf_close(g_TinySoundFont);
    alDeleteSources(1, &source);
    alDeleteBuffers(numBuffers, buffers);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(g_Context);
    alcCloseDevice(g_Device);

    return 0;
}
