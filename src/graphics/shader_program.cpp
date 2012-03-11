#include "shader_program.hpp"

#include "shader_factory.hpp"

namespace crust {
    ShaderProgram::~ShaderProgram()
    {
        destroy();
    }

    void ShaderProgram::create()
    {
        ShaderFactory factory;
        if (vertexShaderHandle_ == 0) {
            vertexShaderHandle_ = factory.compileShader(GL_VERTEX_SHADER, vertexShaderPath_.c_str());
        }
        if (fragmentShaderHandle_ == 0) {
            fragmentShaderHandle_ = factory.compileShader(GL_FRAGMENT_SHADER, fragmentShaderPath_.c_str());
        }
        if (programHandle_ == 0) {
            programHandle_ = factory.linkProgram(vertexShaderHandle_, fragmentShaderHandle_);
        }
    }

    void ShaderProgram::destroy()
    {
        if (programHandle_ != 0) {
            glDeleteProgram(programHandle_);
            programHandle_ = 0;
        }
        if (fragmentShaderHandle_ != 0) {
            glDeleteShader(fragmentShaderHandle_);
            fragmentShaderHandle_ = 0;
        }
        if (vertexShaderHandle_ != 0) {
            glDeleteShader(vertexShaderHandle_);
            vertexShaderHandle_ = 0;
        }
    }
}
