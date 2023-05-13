#include "SoundSource.h"
#include <iostream>
#include "OpenALErrorCheck.h"

SoundSource::SoundSource()
{
	alGenSources(1, &p_Source);
	alSourcei(p_Source, AL_BUFFER, p_Buffer);
	AL_CheckAndThrow();
}

SoundSource::~SoundSource()
{
	alDeleteSources(1, &p_Source);
}

void SoundSource::Play(const ALuint& buffer_to_play)
{
	if (buffer_to_play != p_Buffer)
	{
			p_Buffer = buffer_to_play;
			alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
			AL_CheckAndThrow();
	}
	alSourcePlay(p_Source);
	AL_CheckAndThrow();
}

void SoundSource::Stop()
{
	alSourceStop(p_Source);
	AL_CheckAndThrow();
}

void SoundSource::Pause()
{
	alSourcePause(p_Source);
	AL_CheckAndThrow();
}

void SoundSource::Resume()
{
	alSourcePlay(p_Source);
	AL_CheckAndThrow();
}

void SoundSource::SetBufferToPlay(const ALuint& buffer_to_play)
{
	if (buffer_to_play != p_Buffer)
	{
		p_Buffer = buffer_to_play;
		alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
		AL_CheckAndThrow();
	}
}

void SoundSource::SetLooping(const bool& loop)
{
	alSourcei(p_Source, AL_LOOPING, (ALint)loop);
	AL_CheckAndThrow();
}

void SoundSource::SetPosition(const float& x, const float& y, const float& z)
{
	alSource3f(p_Source, AL_POSITION, x, y, z);
	AL_CheckAndThrow();
}

void SoundSource::SetDirection(const float& x, const float& y, const float& z)
{
	alSource3f(p_Source, AL_DIRECTION, x, y, z);
	AL_CheckAndThrow();
}

bool SoundSource::isPlaying()
{
	ALint playState;
	alGetSourcei(p_Source, AL_SOURCE_STATE, &playState);
	return (playState == AL_PLAYING);
}

ALuint SoundSource::getSourceID()
{
	return p_Source;
}