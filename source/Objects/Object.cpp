#include "Object.h"

Object::Object() //Ponerle game Object
{
	object_model = nullptr;
	object_shader = nullptr;

}

Object::~Object()
{
}


void Object::setModel(Model& model)
{
	object_model = &model;
}

void Object::setShader(Shader& shader)
{
	object_shader = &shader;
}
Shader& Object::getShader() {
	return *object_shader;
}


void Object::Draw()
{
	if (object_model != nullptr && object_shader != nullptr) { //&& transform_model[3][2] < 3.0f && transform_model[3][2] > -2.975f
		object_shader->setMat4("model", transform_model);
		object_model->Draw(*object_shader);
		ResetVectors();
	}
}

glm::vec3 Object::getPosition()
{
	return position;
}

void Object::addAudioSource(SoundSource& audio_source)
{
	sourceID = audio_source.getSourceID();
	object_Asource = make_shared<SoundSource>(audio_source);
	object_Asource->SetPosition(position[0], position[1], position[2]);

}


void Object::addSound(int sound)
{
	sounds.push_back(sound);
}

void Object::Play(const ALuint& buffer_to_play)
{
	//if(!object_Asource->isPlaying())
	object_Asource->Play(buffer_to_play);
}

void Object::Play()
{
	if (object_Asource != nullptr) {
		if (sounds.size() > 0) {
			if (!object_Asource->isPlaying())
				object_Asource->Play(sounds.back());
			else object_Asource->Pause();
		}
	}
}

void Object::SetLooping()
{
	object_Asource->SetLooping(!object_Asource->isLooping());

}

bool Object::isPlaying()
{
	return object_Asource->isPlaying();
}

bool Object::isLooping()
{
	return object_Asource->isLooping();
}

void Object::Update()
{
	transform_model = glm::scale(transform_model, scale_vector);
	transform_model = glm::rotate(transform_model, rotate_angle, rotate_vector);
	transform_model = glm::translate(transform_model, translate_vector);
	position = glm::vec3(transform_model[3][0], transform_model[3][1], transform_model[3][2]);

	if (HasAudioSource()) {
		object_Asource->SetPosition(position[0], position[1], position[2]);
	}

	ResetVectors();
	//object_shader->setMat4("model", transform_model);
	//Draw();
}

void Object::Rotate(float angle, Axis axis)
{
	if (axis == X)
		rotate_vector = glm::vec3(1.0f, 0.0f, 0.0f);
	else if (axis == Y)
		rotate_vector = glm::vec3(0.0f, 1.0f, 0.0f);
	else if (axis == Z)
		rotate_vector = glm::vec3(0.0f, 0.0f, 1.0f);
	rotate_angle = glm::radians(angle);
}

void Object::Translate(float x, float y, float z)
{
	translate_vector = glm::vec3(x, y, z);
}

void Object::Scale(float x, float y, float z)
{
	scale_vector = glm::vec3(x, y, z);
}

void Object::Scale(float multiplier)
{
	scale_vector = glm::vec3(1.0f, 1.0f, 1.0f) * multiplier;
}

void Object::ResetVectors()
{
	scale_vector = glm::vec3(1.0f, 1.0f, 1.0f);
	rotate_vector = glm::vec3(1.0f, 0.0f, 0.0f);
	rotate_angle = glm::radians(0.f);
	translate_vector = glm::vec3(0.0f, 0.0f, 0.0f);
}

bool Object::HasAudioSource()
{
	if (object_Asource != nullptr)
		return true;
	else return false;
}

bool Object::isListener()
{
	return listens;
}

