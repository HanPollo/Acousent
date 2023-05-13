#pragma once
#include <AL\al.h>
#include <vector>

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
	bool UnLoad(const ALuint& bufferId);

private:
	SoundLibrary();
	~SoundLibrary();

	std::vector<ALuint> p_SoundEffectBuffers;
};

