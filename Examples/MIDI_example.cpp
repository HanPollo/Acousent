#include <iostream>
#include <chrono>
#include <thread>
#define TSF_IMPLEMENTATION
#include "../third_party/tsf/tsf.h"
#include "RtMidi.h"

#include "root_directory.h"

#include <AL/al.h>
#include <AL/alc.h>

namespace ac = Acousent;
// ac::getPath("Resources/Audio/SoundFonts/ElectricPiano.sf2").string().c_str()


// Constants
const int BufferSize = 22050;  // Adjust buffer size as needed
std::vector<short> audioBuffer;

// Function to handle MIDI events
void handleMidiEvent(double deltaTime, std::vector<unsigned char>* message, void* userData)
{
    // Extract MIDI event data (channel, note, velocity, etc.)
    int eventType = (message->at(0) >> 4) & 0xf;
    int channel = (message->at(0)) & 0xf;
    int data1 = (message->at(1));
    int data2 = (message->at(2));

    // Log the MIDI event
    std::cout << "MIDI Event: ";
    switch (eventType) {
    case 0x8:
        std::cout << "Note Off";
        break;
    case 0x9:
        std::cout << "Note On";
        break;
        // Handle other MIDI event types as needed
    }
    std::cout << " | Channel: " << channel
        << " | Note: " << data1
        << " | Velocity: " << data2 << std::endl;

    // Play the note using TinySoundFont
    tsf* soundFont = static_cast<tsf*>(userData);
    tsf_channel_note_on(soundFont, channel, data1, data2);
    tsf_render_short(soundFont, audioBuffer.data(), audioBuffer.size(), 0);
}



int main()
{

    // Determine the desired buffer size

    // Initialize the audio buffer with the desired size
    audioBuffer.resize(BufferSize);
    // Initialize OpenAL
    ALCdevice* device = alcOpenDevice(nullptr);
    ALCcontext* context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

    // Initialize TinySoundFont
    tsf* soundFont = tsf_load_filename(ac::getPath("Resources/Audio/SoundFonts/ElectricPiano.sf2").string().c_str());
    if (!soundFont) {
        std::cerr << "Failed to load SoundFont file." << std::endl;
        return 1;
    }

    // Create an RtMidiIn instance
    RtMidiIn midiIn;

    // Check available MIDI ports
    unsigned int portCount = midiIn.getPortCount();
    if (portCount == 0) {
        std::cerr << "No MIDI input ports available." << std::endl;
        return 1;
    }

    // Open the first MIDI input port
    midiIn.openPort(0);

    // Set the MIDI event handler
    midiIn.setCallback(&handleMidiEvent, soundFont);

    // Ignore any sysex, timing, or active sensing messages
    midiIn.ignoreTypes(true, true, true);

    // Log initialization and SoundFont name
    std::cout << "TinySoundFont initialized." << std::endl;
    std::cout << "SoundFont: " << ac::getPath("Resources/Audio/SoundFonts/ElectricPiano.sf2").string().c_str() << std::endl;

    // Create OpenAL source and buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);

    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);

    // Set the audio rendering callback
    tsf_set_output(soundFont, TSF_MONO, 44100, 0); //sample rate
    alBufferData(buffer, AL_FORMAT_MONO16, audioBuffer.data(), sizeof(audioBuffer), 44100);


    // Application loop
    bool exit = false;
    while (!exit) {
        // Check for MIDI events
        std::vector<unsigned char> message;
        double deltaTime = midiIn.getMessage(&message);

        // Process MIDI event if received
        if (!message.empty()) {
            handleMidiEvent(deltaTime, &message, soundFont);
            alSourcePlay(source);
        }

        // Simulate your application logic here
        // ...

        // Check if the escape key is pressed (assumes terminal input)
        if (std::cin.peek() == '\x1b') {
            exit = true;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        // Sleep for a short duration to avoid excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Clean up resources
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    tsf_close(soundFont);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}
