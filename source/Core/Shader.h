//
// Created by pollo on 4/27/2022.
//

#ifndef BOOMBOOMENGINE_SHADERCLASS_H
#define BOOMBOOMENGINE_SHADERCLASS_H

#include "glad/glad.h"
#include"glm/glm.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


std::string get_file_contents(const char* filename);

class Shader{
    public:
        // Reference ID of the Shader Program
        unsigned int ID;
        // Constructor that build the Shader Program from 2 different shaders
        Shader(const char* vertexPath, const char* fragmentPath);

        // Activates the Shader Program
        void use();

        // utility uniform functions
        // ------------------------------------------------------------------------
        void setBool(const std::string &name, bool value) const;
        // ------------------------------------------------------------------------
        void setInt(const std::string &name, int value) const;
        // ------------------------------------------------------------------------
        void setFloat(const std::string &name, float value) const;
        // ------------------------------------------------------------------------
        void setVec2(const std::string &name, const glm::vec2 &value) const;
        void setVec2(const std::string &name, float x, float y) const;
        // ------------------------------------------------------------------------
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setVec3(const std::string &name, float x, float y, float z) const;
        // ------------------------------------------------------------------------
        void setVec4(const std::string &name, const glm::vec4 &value) const;
        void setVec4(const std::string &name, float x, float y, float z, float w);
        // ------------------------------------------------------------------------
        void setMat2(const std::string &name, const glm::mat2 &mat) const;
        // ------------------------------------------------------------------------
        void setMat3(const std::string &name, const glm::mat3 &mat) const;
        // ------------------------------------------------------------------------
        void setMat4(const std::string &name, const glm::mat4 &mat) const;
        // Deletes the Shader Program
        void Delete();

    private:
        // Checks if the different Shaders have compiled properly
        void checkCompileErrors(GLuint shader, std::string type);
};

#endif //BOOMBOOMENGINE_SHADERCLASS_H
