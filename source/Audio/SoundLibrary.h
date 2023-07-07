#pragma once
#include <AL\al.h>
#include <AL/alext.h>

#include <vector>
#include <inttypes.h>

#include "../root_directory.h"

#include <string>
#include <stdexcept>
#include <fstream>


/// <summary>
/// SoundLibrary: Singleton class that keeps track of the shorter sounds
/// we've loaded into memory. Use the Play function from an instantiation of the
/// SoundSource on an ID returned from the load function.
/// 
/// load function: Use the SE_LOAD(...) function to add sounds, programmer should keep track of
/// the returned ALuint to use the sound.
/// 
/// unload function: Use SE_UNLOAD(...) remove sounds from memory. 
/// 
/// </summary>

#define SE_LOAD SoundLibrary::Get()->Load
#define SE_UNLOAD SoundLibrary::Get()->UnLoad

class SoundLibrary
{
public:
	static SoundLibrary* Get();

	ALuint Load(const char* filename);
	ALuint LoadMIDI(const char* midi, const char* sf2);
	bool UnLoad(const ALuint& bufferId);

private:
	SoundLibrary();
	~SoundLibrary();

	std::vector<ALuint> p_SoundEffectBuffers;

	// Sample rate es la cantidad de samples que se requieren. Mientras mas samples mejor la calidad del audio pero mas memoria se utiliza.
	// En general se utiliza 44100 samples o 44.1kHz de Sample Rate ya que este es el sample rate de los CDs clasicos.
	// En caso de que se requiera una mejor calidad de audio o peor, si se pierden notas se debe incrementar el sample Rate.
	const int sampleRate = 44100.0;

	
};

