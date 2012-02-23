#include "shader_factory.hpp"

#include "error.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace crust {
    GLuint ShaderFactory::compileShader(GLenum type, char const *file)
    {
        GLchar const *source = loadSource(file);
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, 0);
        glCompileShader(shader);
        GLint compileStatus = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus != GL_TRUE) {
            std::cerr << getShaderInfoLog(shader) << std::endl;

            std::stringstream message;
            message << "Failed to compile shader from file: " << file;
            throw Error(message.str());            
        }
        return shader;
    }

    GLuint ShaderFactory::linkProgram(GLuint shader1, GLuint shader2, GLuint shader3)
    {
        GLuint program = glCreateProgram();
        glAttachShader(program, shader1);
        if (shader2) {
            glAttachShader(program, shader2);
        }
        if (shader3) {
            glAttachShader(program, shader3);
        }
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            std::cerr << getProgramInfoLog(program) << std::endl;

            std::stringstream message;
            message << "Failed to link shader program";
            throw Error(message.str());
        }
        return program;
    }

    GLchar const *ShaderFactory::loadSource(char const *file)
    {
        std::ifstream stream(file);
        if (!stream.is_open()) {
            std::stringstream message;
            message << "Failed to open shader file: " << file;
            throw Error(message.str());
        }
        stream.seekg(0, std::ifstream::end);
        std::size_t size(stream.tellg());        
        stream.seekg(0, std::ifstream::beg);
        buffer_.resize(size + 1);
        stream.read(&buffer_.front(), size);
        if (std::size_t(stream.tellg()) != size) {
            std::stringstream message;
            message << "Failed to read shader file: " << file;
            throw Error(message.str());
        }
        buffer_.back() = 0;
        return reinterpret_cast<GLchar const *>(&buffer_.front());
    }

    GLchar const *ShaderFactory::getShaderInfoLog(GLuint shader)
    {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if (length) {
            buffer_.resize(length);
            glGetShaderInfoLog(shader, GLsizei(buffer_.size()), 0, reinterpret_cast<GLchar *>(&buffer_.front()));
        } else {
            buffer_.resize(1, 0);
        }
        return reinterpret_cast<GLchar const *>(&buffer_.front());
    }

    GLchar const *ShaderFactory::getProgramInfoLog(GLuint program)
    {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        if (length) {
            buffer_.resize(length);
            glGetProgramInfoLog(program, GLsizei(buffer_.size()), 0, reinterpret_cast<GLchar *>(&buffer_.front()));
        } else {
            buffer_.resize(1, 0);
        }
        return reinterpret_cast<GLchar const *>(&buffer_.front());
    }
}
