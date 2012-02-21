#include "shader_factory.hpp"

#include "error.hpp"

#include <fstream>
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
        source_.resize(size + 1);
        stream.read(&source_.front(), size);
        if (std::size_t(stream.tellg()) != size) {
            std::stringstream message;
            message << "Failed to read shader file: " << file;
            throw Error(message.str());
        }
        source_.back() = 0;
        return reinterpret_cast<GLchar const *>(&source_.front());
    }
}
