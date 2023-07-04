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

	return buffer;
}



/// <summary>
/// Loads the sound file into memory.
/// </summary>
/// <param name="filename">path to the file to load</param>
/// <returns>access id</returns>
ALuint SoundLibrary::LoadMIDI(const char* filename)
{

	ALenum err;
	ALuint buffer;

	// Load the MIDI file
	tml_message* context = tml_load_filename(filename);
	if (context == NULL)
	{
		throw std::runtime_error(std::string("Failed to open file: ") + filename);
	}

	// Initialize the TinySoundFont library
	tsf* soundfont = tsf_load_filename(ac::getPath("Resources/Audio/SoundFonts/ElectricPiano.sf2").string().c_str());
	if (!soundfont)
	{
		tml_free(context);
		throw std::runtime_error("Failed to load SoundFont");
	}

	// Set the sample rate
	const unsigned int sample_rate = 44100; // Set your desired sample rate

	// Generate audio data from the MIDI file using the SoundFont
	tsf_set_output(soundfont, TSF_MONO, sample_rate, 0);


	
	// TML Info Variables
	int channels;
	int out_programs;
	int out_total_notes;
	unsigned int out_first_note;
	unsigned int time_length;
	tml_get_info(context, &channels, &out_programs, &out_total_notes, &out_first_note, &time_length);

	cout << "Channels: " << channels << endl;
	cout << "Programs: " << out_programs << endl;
	cout << "Total notes: " << out_total_notes << endl;
	cout << "First note: " << out_first_note << endl;
	cout << "Time length: " << time_length << endl;
	
	const int frames = time_length * sample_rate / 1000; // Porque time_length esta en milisegundos y samplerate en Hz.
	
	//const int frames = tml_get_remaining_time(&context, 0, INT_MAX) * sample_rate / 1000;
	const int size = sizeof(float) * frames * 1; // MONO audio data (Stereo seria *2)

	cout << "Frames: " << frames << endl;
	cout << "Size: " << size << endl;

	float* data = new float[size];
	tsf_render_float(soundfont, data, frames, 0);

	enum class Format {
		Mono8 = AL_FORMAT_MONO8,
		Mono16 = AL_FORMAT_MONO16,
		Stereo8 = AL_FORMAT_STEREO8,
		Stereo16 = AL_FORMAT_STEREO16,
		Mono32 = AL_FORMAT_MONO_FLOAT32,
		Stereo32 = AL_FORMAT_STEREO_FLOAT32
	};

	// Buffer the audio data into a new buffer object
	buffer = 0;
	alGenBuffers(1, &buffer);
	auto fmt = 1;
	Format format;
	fmt == 1 ? format = Format::Mono32 :  format = Format::Stereo32;
	alBufferData(buffer, static_cast<ALenum>(format), data, size, sample_rate); // ACA el error

	// Clean up memory
	delete[] data;
	tsf_close(soundfont);
	tml_free(context);

	// Check if an error occurred and clean up if so
	cout << "Format: " << static_cast<ALenum>(format) << endl;
	err = alGetError();
	if (err != AL_NO_ERROR)
	{
		fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}

	p_SoundEffectBuffers.push_back(buffer); // Add to the list of known buffers

	return buffer;
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
