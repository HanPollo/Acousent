#ifndef OBJECT_H
#define OBJECT_H
#pragma once

#include "../Rendering/Model.h"
#include "../Audio/SoundSource.h"
#include <list>
#include "../root_directory.h"

namespace ac = Acousent;
class Object
{
	enum Axis {
		X,
		Y,
		Z
	};

public:
	Object();
	virtual ~Object();

	void setModel(Model& model);
	void setShader(Shader& shader);

	Shader& getShader();

	void positionObject(float x, float y, float z);
	void Draw();
	
	glm::vec3 getPosition();
	void addAudioSource(SoundSource& audio_source);

	void addSound(int sound);
	void Play(const ALuint& buffer_to_play);
	void Play();
	void SetLooping();
	bool isPlaying();
	bool isLooping();

	virtual void Update();

	void Rotate(float angle, Axis axis);
	void Translate(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Scale(float multiplier);

	virtual void ResetVectors();

	bool HasAudioSource();
	bool isListener();



protected:

	Model* object_model;
	Shader* object_shader;
	shared_ptr<SoundSource> object_Asource = nullptr;
	ALuint sourceID;
	bool listens = false;

	glm::mat4 transform_model = glm::mat4(1.0f);

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 translate_vector = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotate_vector = glm::vec3(1.0f, 0.0f, 0.0f);
	float rotate_angle = glm::radians(0.f);
	glm::vec3 scale_vector = glm::vec3(1.0f, 1.0f, 1.0f);

	list<int> sounds;

	float rot_speed = 1.f;
	float move_speed = 1.f;
	float scale_speed = 0.01f;




};



#endif // OBJECT_H