#pragma once

#include <string>

namespace PxlUI {
    struct GLShaderUtil {
        struct ShaderProgramSource {
            std::string vertexSource;
            std::string fragmentSource;
        };

        static ShaderProgramSource parseShader(const std::string &filepath);

        static int compileShader(unsigned int type, const std::string &source);

        static int createShaderProgram(const std::string &vertexShader, const std::string &fragmentShader);
    };
}// namespace PxlUI
