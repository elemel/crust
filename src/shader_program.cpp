#include "shader_program.hpp"

#include "shader_factory.hpp"

namespace crust {
    ShaderProgram::~ShaderProgram()
    {
        if (programHandle_ != 0) {
            glDeleteProgram(programHandle_);
        }
        if (fragmentShaderHandle_ != 0) {
            glDeleteShader(fragmentShaderHandle_);
        }
        if (vertexShaderHandle_ != 0) {
            glDeleteShader(vertexShaderHandle_);
        }
    }

    void ShaderProgram::create()
    {
        createVertexShader();
        createFragmentShader();
        createProgram();
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

    void ShaderProgram::createVertexShader()
    {
        if (vertexShaderHandle_ == 0) {
            ShaderFactory factory;
            vertexShaderHandle_ = factory.compileShader(GL_VERTEX_SHADER, vertexShaderPath_.c_str());
        }
    }

    void ShaderProgram::createFragmentShader()
    {
        if (fragmentShaderHandle_ == 0) {
            ShaderFactory factory;
            fragmentShaderHandle_ = factory.compileShader(GL_FRAGMENT_SHADER, fragmentShaderPath_.c_str());
        }
    }

    void ShaderProgram::createProgram()
    {
        if (programHandle_ == 0) {
            ShaderFactory factory;
            programHandle_ = factory.linkProgram(vertexShaderHandle_, fragmentShaderHandle_);
        }
    }
}
