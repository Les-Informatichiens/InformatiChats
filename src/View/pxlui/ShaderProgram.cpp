#include "ShaderProgram.h"
#include "GLShaderUtil.h"

#include "glm/gtc/type_ptr.hpp"

#include "GL/glew.h"

PxlUI::ShaderProgram::ShaderProgram(const std::string &iFilePath)
    : mFilePath(iFilePath), mProgramId(0)
{
    GLShaderUtil::ShaderProgramSource wSource = GLShaderUtil::parseShader(iFilePath);
    mProgramId = GLShaderUtil::createShaderProgram(wSource.vertexSource, wSource.fragmentSource);
}

PxlUI::ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(mProgramId);
}

uint32_t PxlUI::ShaderProgram::getProgramId()
{
    return mProgramId;
}

void PxlUI::ShaderProgram::bind()
{
    glUseProgram(mProgramId);
}

void PxlUI::ShaderProgram::unbind()
{
    glUseProgram(0);
}

void PxlUI::ShaderProgram::setInt(const std::string &name, int value)
{
    PxlUI::ShaderProgram::uploadUniformInt(name, value);
}

void PxlUI::ShaderProgram::setIntArray(const std::string &name, int *values, uint32_t count)
{
    PxlUI::ShaderProgram::uploadUniformIntArray(name, values, count);
}

void PxlUI::ShaderProgram::setFloat(const std::string &name, float value)
{
    PxlUI::ShaderProgram::uploadUniformFloat(name, value);
}

void PxlUI::ShaderProgram::setFloat2(const std::string &name, const glm::vec2 &value)
{
    PxlUI::ShaderProgram::uploadUniformFloat2(name, value);
}

void PxlUI::ShaderProgram::setFloat3(const std::string &name, const glm::vec3 &value)
{
    PxlUI::ShaderProgram::uploadUniformFloat3(name, value);
}

void PxlUI::ShaderProgram::setFloat4(const std::string &name, const glm::vec4 &value)
{
    PxlUI::ShaderProgram::uploadUniformFloat4(name, value);
}

void PxlUI::ShaderProgram::setMat4(const std::string &name, const glm::mat4 &value)
{
    PxlUI::ShaderProgram::uploadUniformMat4(name, value);
}

void PxlUI::ShaderProgram::uploadUniformInt(const std::string &name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void PxlUI::ShaderProgram::uploadUniformIntArray(const std::string &name, int *values, uint32_t count)
{
    glUniform1iv(getUniformLocation(name), count, values);
}

void PxlUI::ShaderProgram::uploadUniformFloat(const std::string &name, float value)
{
    glUniform1f(getUniformLocation(name), value);
}

void PxlUI::ShaderProgram::uploadUniformFloat2(const std::string &name, const glm::vec2 &value)
{
    glUniform2f(getUniformLocation(name), value.x, value.y);
}

void PxlUI::ShaderProgram::uploadUniformFloat3(const std::string &name, const glm::vec3 &value)
{
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void PxlUI::ShaderProgram::uploadUniformFloat4(const std::string &name, const glm::vec4 &value)
{
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void PxlUI::ShaderProgram::uploadUniformMat3(const std::string &name, const glm::mat3 &matrix)
{
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void PxlUI::ShaderProgram::uploadUniformMat4(const std::string &name, const glm::mat4 &matrix)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

#include <iostream>

int32_t PxlUI::ShaderProgram::getUniformLocation(const std::string &iName)
{

    if (mUniformLocationCache.find(iName) != mUniformLocationCache.end())
        return mUniformLocationCache[iName];

    int32_t wLocation = glGetUniformLocation(mProgramId, iName.c_str());
    std::cout << "Location of " << iName << " is " << wLocation << std::endl;
    //    _ASSERT(wLocation != -1);

    mUniformLocationCache[iName] = wLocation;
    return wLocation;
}

PxlUI::ShaderProgram::ShaderProgram(const std::string &iVertexShader, const std::string &iFragmentShader)
{
    mProgramId = GLShaderUtil::createShaderProgram(iVertexShader, iFragmentShader);
}
