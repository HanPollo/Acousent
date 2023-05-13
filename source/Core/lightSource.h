#include "glad/glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "BasicMesh.h"
#include "shaderClass.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Light
{
	Light(glm::vec3 position);
	~Light();

	Shader lightShader;
	glm::vec3 position;
	bool isOn = false;


	void setPosition(glm::vec3 pos);
	void On();
	void Off();

};



