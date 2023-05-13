#pragma once
#include <AL\al.h>
#include <glm/glm.hpp>
class SoundSource
{
public:
	SoundSource();
	~SoundSource();

	void Play(const ALuint& buffer_to_play);
	void Stop();
	void Pause();
	void Resume();

	// Setters
	void SetBufferToPlay(const ALuint& buffer_to_play);
	void SetLooping(const bool& loop);
	void SetPosition(const float& x, const float& y, const float& z);
	void SetDirection(const float& x, const float& y, const float& z);

	// Getters
	ALuint GetBufferPlaying();
	glm::vec3 GetPosition();

	bool isPlaying();
	ALuint getSourceID();

private:
	ALuint p_Source;
	ALuint p_Buffer = 0;
};

