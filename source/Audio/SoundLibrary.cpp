#include "SoundLibrary.h"

#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

#define TSF_IMPLEMENTATION
#include "tsf.h"

#define TML_IMPLEMENTATION
#include "tml.h"

#include <iostream>


namespace ac = Acousent;
using namespace std;


// El soundfont que se utilizara. Esto tiene que ser global para que se pueda ir cambiando entre distintos presets (instrumentos) 
// dentro del mismo archivo .sf2 
//static tsf* g_TinySoundFont;

// Lo mismo para los MIDI ya que estos pueden contener varios tracks
//static double g_Msec;  // Tiempo actual de playback
//static tml_message* g_MidiMessage;  // Proximo mensaje MIDI a leer

// Un entero estilo contador para ver cuantas iteraciones de los loops se estan realizando asi compararlo con la cantidad necesaria.
int test_int = 0;



// Esta es la funcion principal para la lectura de archivos MIDI. Lee los archivos y crea un buffer que lo guarda en el puntero audioStream.
void ProcessMIDIAndRender(float* audioStream, int streamLength, tml_message* tml, tsf* tsf)
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

		// La siguiente linea de codigo crea el audio a partir de valores float y los guarda en el puntero audioStream.
		// Esto lo realiza.
		tsf_render_float(tsf, audioStream, sampleBlock, 0);
		// Contador para efectos de Debug
		test_int++;
		if (test_int > 2267) {
			//cout << "Test_int es maypr a 2267" << endl;
			//auto float_size = sizeof(float);
			//cout << "Float size: " << float_size << endl;
		}
		// Se deben calcular cuantos bytes se procesaron para asi avanzar el puntero al siguiente lugar en memoria donde se continuara la cancion.
		int blockSizeBytes = sampleBlock * (1 * sizeof(float));

		// Incrementar el puntero por la cantidad de samples procesados.
		audioStream += blockSizeBytes / sizeof(unsigned int);

	}
}

// Esta es la funcion principal para la lectura de tracks especificos archivos MIDI. Lee los archivos y crea un buffer que lo guarda en el puntero audioStream.
void ProcessTrackAndRender(float* audioStream, int streamLength, int trackNumber, int presetNumber, tml_message* tml, tsf* tsf)
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
			tsf_channel_set_presetindex(tsf, trackNumber, presetNumber);
			// Revisa si el mensaje midi es el que se desea reproducir.
			if (tml->channel == trackNumber)
			{
				switch (tml->type)
				{
				case TML_NOTE_ON:
					// Nota ON
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

		// La siguiente linea de codigo crea el audio a partir de valores float y los guarda en el puntero audioStream.
		// Esto lo realiza.
		tsf_render_float(tsf, audioStream, sampleBlock, 0);
		// Contador para efectos de Debug
		test_int++;
		if (test_int > 2267) {
			//cout << "Test_int es maypr a 2267" << endl;
			//auto float_size = sizeof(float);
			//cout << "Float size: " << float_size << endl;
		}
		// Se deben calcular cuantos bytes se procesaron para asi avanzar el puntero al siguiente lugar en memoria donde se continuara la cancion.
		int blockSizeBytes = sampleBlock * (1 * sizeof(float));

		// Incrementar el puntero por la cantidad de samples procesados.
		audioStream += blockSizeBytes / sizeof(unsigned int);

	}
}


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


/// <summary>
/// Static class access.
/// </summary>
/// <returns>A pointer to the only instantiation allowed.</returns>
SoundLibrary* SoundLibrary::Get()
{
	static SoundLibrary* sndbuf = new SoundLibrary();
	return sndbuf;
}

/// <summary>
/// Loads the sound file into memory.
/// </summary>
/// <param name="filename">path to the file to load</param>
/// <returns>access id</returns>
ALuint SoundLibrary::Load(const char* filename)
{
	while (loading) {
		// busy waiting
	}
	loading = true;

	ALenum err;
	ALuint buffer;
	unsigned int channels;
	unsigned int sample_rate;
	drwav_uint64 frame_count;

	drwav_int16* data = drwav_open_file_and_read_pcm_frames_s16(
		filename,
		&channels,
		&sample_rate,
		&frame_count,
		nullptr);

	if (!data) {
		throw std::runtime_error(std::string("Failed to open file: ") + filename);
	}
	enum class Format {
		Mono8 = AL_FORMAT_MONO8,
		Mono16 = AL_FORMAT_MONO16,
		Stereo8 = AL_FORMAT_STEREO8,
		Stereo16 = AL_FORMAT_STEREO16
	};
	const auto fmt = channels == 1 ? Format::Mono16 : Format::Stereo16;
	const auto size = sizeof(drwav_int16) * frame_count;

	/* Buffer the audio data into a new buffer object, then free the data and
	 * close the file.
	 */
	buffer = 0;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, static_cast<ALenum>(fmt), data, size, sample_rate);

	drwav_free(data, nullptr);


	//Hasta Aca

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR)
	{
		fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}

	p_SoundEffectBuffers.push_back(buffer);  // add to the list of known buffers

	loading = false;
	return buffer;
}



/// <summary>
/// Loads the sound file into memory.
/// </summary>
/// <param name="filename">path to the file to load</param>
/// <returns>access id</returns>
ALuint SoundLibrary::LoadMIDI(const char* midi, const char* sf2, bool singleTrack, int trackNum, int soundNum)
{
	if (!loading) {
		loading = true;
		ALenum err;
		tml_message* TinyMidiLoader = nullptr;
		tsf* tsf = nullptr;

		// Cargar el soundfont desde el archivo.
		tsf = tsf_load_filename(sf2);
		if (!tsf)
		{
			fprintf(stderr, "Could not load SoundFont\n");
			return 1;
		}

		// Inicializa el preset en el canal especial numero 10 del MIDI para utilizar el banco de sonidos de percucion del soundfont (128) si lo tiene
		tsf_channel_set_bank_preset(tsf, 9, 128, 0);

		// Le indicamos a tsf los parametros de rendering del audio.
		tsf_set_output(tsf, TSF_MONO, sampleRate, 0.0f); // Hay que 

		// Cargamos el archivo MIDI
		TinyMidiLoader = tml_load_filename(midi);
		if (!TinyMidiLoader)
		{
			fprintf(stderr, "Could not load MIDI file\n");
			return 1;
		}

		// Inicializamos el midi loader global en el primer mensaje de nuestro archivo cargado.
		// Esto para que la funcion pueda utilizar el midi correcto.
		//g_MidiMessage = TinyMidiLoader;

		// Calculamos el largo necesario del buffer de audio.
		auto streamLength = CalculateStreamLength(sampleRate, TinyMidiLoader);
		// Alocamos la memoria necesaria para el buffer completo en forma de arreglo.
		float* audioStream = new float[streamLength / sizeof(float)];

		// Procesamos el midi con el soundfont elegido y lo guardamos en audioStream
		if (!singleTrack)
			ProcessMIDIAndRender(audioStream, streamLength, TinyMidiLoader, tsf);
		else
			ProcessTrackAndRender(audioStream, streamLength, trackNum, soundNum, TinyMidiLoader, tsf);


		// TML Info Variables
		int channels;
		int out_programs;
		int out_total_notes;
		unsigned int out_first_note;
		unsigned int time_length;
		tml_get_info(TinyMidiLoader, &channels, &out_programs, &out_total_notes, &out_first_note, &time_length);

		/*
		cout << "Channels: " << channels << endl;
		cout << "Programs: " << out_programs << endl;
		cout << "Total notes: " << out_total_notes << endl;
		cout << "First note: " << out_first_note << endl;
		cout << "Time length: " << time_length << endl;
		*/
		// Creamos el buffer de OpenAL utilizando el buffer audioStream para la informacion del audio.
		ALuint buffer;
		alGenBuffers(1, &buffer);
		alBufferData(buffer, AL_FORMAT_MONO_FLOAT32, audioStream, static_cast<ALsizei>(streamLength), sampleRate);

		p_SoundEffectBuffers.push_back(buffer); // Add to the list of known buffers

		//Cleanup
		delete[] audioStream;
		tsf_close(tsf);
		tml_free(TinyMidiLoader);

		loading = false;
		return buffer;
	}
}



/// <summary>
/// Unloads the sound file from memory.
/// </summary>
/// <param name="buffer"></param>
/// <returns></returns>
bool SoundLibrary::UnLoad(const ALuint& buffer)
{
	auto it = p_SoundEffectBuffers.begin();
	while (it != p_SoundEffectBuffers.end())
	{
		if (*it == buffer)
		{
			alDeleteBuffers(1, &*it);

			it = p_SoundEffectBuffers.erase(it);

			return true;
		}
		else {
			++it;
		}
	}
	return false;  // couldn't find to remove
}

/// <summary>
/// Class initialization.
/// </summary>
SoundLibrary::SoundLibrary()
{
	p_SoundEffectBuffers.clear();
}

/// <summary>
/// Class Destructor. Removes all loaded sounds from memory.
/// </summary>
SoundLibrary::~SoundLibrary()
{
	alDeleteBuffers((ALsizei)p_SoundEffectBuffers.size(), p_SoundEffectBuffers.data());

	p_SoundEffectBuffers.clear();
}
