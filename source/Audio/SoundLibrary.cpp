#include "SoundLibrary.h"
#include <inttypes.h>


#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

#include <string>
#include <stdexcept>

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
