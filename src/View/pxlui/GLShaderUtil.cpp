#include "GLShaderUtil.h"

#include "GL/glew.h"

#include <fstream>
#include <iostream>
#include <sstream>

PxlUI::GLShaderUtil::ShaderProgramSource PxlUI::GLShaderUtil::parseShader(const std::string &filepath)
{

    std::stringstream ss[2];
    for (const auto &shaderType: {"vert", "frag"})
    {
        std::ifstream stream(std::string(filepath) + "/shader." + shaderType);

        enum class ShaderType
        {
            NONE = -1,
            VERTEX = 0,
            FRAGMENT = 1
        };

        std::string line;
        ShaderType type = shaderType == "vert" ? ShaderType::VERTEX : ShaderType::FRAGMENT;

        while (std::getline(stream, line))
        {
            ss[(int) type] << line << '\n';
        }
    }
    return {ss[0].str(), ss[1].str()};
}

int PxlUI::GLShaderUtil::compileShader(unsigned int type, const std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == false)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *) _malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!"
                  << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

int PxlUI::GLShaderUtil::createShaderProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
