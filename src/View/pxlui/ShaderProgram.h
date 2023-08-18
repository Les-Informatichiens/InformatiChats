#pragma once

#include "glm/glm.hpp"
#include <string>
#include <unordered_map>

namespace PxlUI {
    class ShaderProgram
    {
    public:
        ShaderProgram(const std::string &iFilePath);
        ShaderProgram(const std::string &iVertexShader, const std::string &iFragmentShader);

        ~ShaderProgram();

        uint32_t getProgramId();

        void bind();
        void unbind();

        void setInt(const std::string &name, int value);
        void setIntArray(const std::string &name, int *values, uint32_t count);
        void setFloat(const std::string &name, float value);
        void setFloat2(const std::string &name, const glm::vec2 &value);
        void setFloat3(const std::string &name, const glm::vec3 &value);
        void setFloat4(const std::string &name, const glm::vec4 &value);
        void setMat4(const std::string &name, const glm::mat4 &value);
        void uploadUniformInt(const std::string &name, int value);
        void uploadUniformIntArray(const std::string &name, int *values, uint32_t count);
        void uploadUniformFloat(const std::string &name, float value);
        void uploadUniformFloat2(const std::string &name, const glm::vec2 &value);
        void uploadUniformFloat3(const std::string &name, const glm::vec3 &value);
        void uploadUniformFloat4(const std::string &name, const glm::vec4 &value);
        void uploadUniformMat3(const std::string &name, const glm::mat3 &matrix);
        void uploadUniformMat4(const std::string &name, const glm::mat4 &matrix);

    private:
        int32_t getUniformLocation(const std::string &iName);

        std::string mFilePath;
        uint32_t mProgramId;
        std::unordered_map<std::string, int32_t> mUniformLocationCache;
    };
}// namespace PxlUI
