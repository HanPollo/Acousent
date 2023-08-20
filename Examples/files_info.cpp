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
#include <string>


namespace ac = Acousent;
using namespace std;

vector<string> nombres;
vector<int> track_nums;

int sampleRate = 44100.0;

// Esta funcion utiliza el archivo midi y el sample rate para poder dividir el archivo en bloques.
int CalculateStreamLength(double sampleRate, tml_message* tml)
{
	int maxTimestamp = 0;

	// Itera los mensajes MIDI y encuentra el mayor largo de los tracks. (Esto sera el largo completo ya que los tracks siempre parten del principio).
	for (tml_message* message = tml; message != NULL; message = message->next)
	{
		if (message->time > maxTimestamp)
		{
			maxTimestamp = message->time;
		}
	}

	// Se calcula streamLength (largo del puntero para el buffer) utilizando el sample rate y el mayor tiempo de los tracks 
	// (Dividio por 1000 ya que se requieren segundos)
	int streamLength = (int)(maxTimestamp * sampleRate / 1000.0) * (1 * sizeof(float)); // 1 Channel

	return streamLength;
}

// Esta es la funcion principal para la lectura de archivos MIDI. Lee los archivos y crea un buffer que lo guarda en el puntero audioStream.
void ProcessMIDIAndRender(int streamLength, tml_message* tml, tsf* tsf)
{
	double g_Msec = 0;
	// El archivo MIDI se tiene que leer por bloques.
	int sampleBlock, sampleCount = streamLength / (1 * sizeof(float)); // La multiplicacion por 1 es por la cantidad de canales (MONO en este caso)

	// Iteracion por bloque
	for (sampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK; sampleCount > 0; sampleCount -= sampleBlock)
	{
		// Esto para que cuando el bloque sea mayor a los samples restantes no solo utilice un bloque del tamano de los restantes.
		if (sampleBlock > sampleCount)
			sampleBlock = sampleCount;

		// Se procesa el MIDI con los parametros necesarios y para cada senal que entrega el MIDI el soundfont debe realizar una accion.
		for (g_Msec += sampleBlock * (1000.0 / 44100.0); tml && g_Msec >= tml->time; tml = tml->next)
		{
			switch (tml->type)
			{
			case TML_PROGRAM_CHANGE:
				// Si el MIDI tiene algun numero de preset especifico para elegir instrumento se detecta aca y se cambia el preset del soundfont
				tsf_channel_set_presetnumber(tsf, tml->channel, tml->program, (tml->channel == 9));
				break;
			case TML_NOTE_ON:
				// Nota ON
				if (std::find(track_nums.begin(), track_nums.end(), tml->channel) != track_nums.end()) {
				}
				else {
					track_nums.push_back(tml->channel);
					auto preset_num = tsf_channel_get_preset_index(tsf, tml->channel);
					auto nombre = tsf_get_presetname(tsf, preset_num);
					nombres.push_back(nombre);
				}
				tsf_channel_note_on(tsf, tml->channel, tml->key, tml->velocity / 127.0f);
				break;
			case TML_NOTE_OFF:
				// Nota OFF
				tsf_channel_note_off(tsf, tml->channel, tml->key);
				break;
			case TML_PITCH_BEND:
				// Si hay cambios de tonos se deben ajustar en el soundfont para tener la nota correcta para esa nota en el MIDI.
				tsf_channel_set_pitchwheel(tsf, tml->channel, tml->pitch_bend);
				break;
			case TML_CONTROL_CHANGE:
				// La libreria tiny sound font recibe los mensajes de controllador MIDI si esque hay cambios.
				tsf_channel_midi_control(tsf, tml->channel, tml->control, tml->control_value);
				break;
			}
		}

	}
}

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
	auto streamLength = CalculateStreamLength(sampleRate, midi);
	ProcessMIDIAndRender(streamLength, midi, soundFont);


    
    std::cout << endl << "SoundFont Information:" << std::endl;
    int presets_count = tsf_get_presetcount(soundFont);
    cout << "Presets: " << presets_count << endl;
    for (int i = 0; i < presets_count; i++)
    {
        cout << "Preset: " << i << endl << "    ";
        cout << "Name: " << tsf_get_presetname(soundFont, i) << endl;
    }
    
	std::cout << endl << "MIDI Information:" << std::endl;
	std::cout << "Number of Tracks with notes: " << tracks << std::endl;
	std::cout << "Number of Programs: " << out_programs << std::endl;

	for (int i = 0; i < track_nums.size(); i++)
	{
		cout << "Midi Track id: " << track_nums[i] << endl << "    ";
		cout << "Name of default preset: " << nombres[i] << endl;
	}

    tsf_close(soundFont);
    tml_free(midi);


    return 0;
}
