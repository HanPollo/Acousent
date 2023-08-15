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

int main()
{


    std::string midiFileName, soundFontFileName;
    std::cout << "Enter the MIDI file name: ";
    std::cin >> midiFileName;

    std::cout << "Enter the SoundFont file name: ";
    std::cin >> soundFontFileName;

    tsf* soundFont = tsf_load_filename(ac::getPath("Resources/Audio/SoundFonts/" + soundFontFileName).string().c_str());
    if (!soundFont)
    {
        std::cout << "Failed to load the SoundFont file." << std::endl;
        return 1;
    }

    tml_message* midi = tml_load_filename(ac::getPath("Resources/Audio/MIDI/" + midiFileName).string().c_str());
    if (!midi)
    {
        tsf_close(soundFont);
        std::cout << "Failed to load the MIDI file." << std::endl;
        return 1;
    }

    // TML Info Variables
    int tracks;
    int out_programs;
    int out_total_notes;
    unsigned int out_first_note;
    unsigned int time_length;
    tml_get_info(midi, &tracks, &out_programs, &out_total_notes, &out_first_note, &time_length);

    
    std::cout << "MIDI Information:" << std::endl;
    std::cout << "Number of Tracks: " << tracks << std::endl;
    std::cout << "Number of Programs: " << out_programs << std::endl;

    
    std::cout << endl << "SoundFont Information:" << std::endl;
    int presets_count = tsf_get_presetcount(soundFont);
    cout << "Presets: " << presets_count << endl;
    for (int i = 0; i < presets_count; i++)
    {
        cout << "Preset: " << i << endl << "    ";
        cout << "Name: " << tsf_get_presetname(soundFont, i) << endl;
    }
    

    tsf_close(soundFont);
    tml_free(midi);


    return 0;
}
