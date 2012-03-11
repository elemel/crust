#ifndef CRUST_SHADER_PROGRAM_HPP
#define CRUST_SHADER_PROGRAM_HPP

#include <string>
#include <SDL/SDL_opengl.h>

namespace crust {
    class ShaderProgram {
    public:
        ShaderProgram() :
            vertexShaderHandle_(0),
            fragmentShaderHandle_(0),
            programHandle_(0)
        { }
        
        ~ShaderProgram();

        char const *getVertexShaderPath() const
        {
            return vertexShaderPath_.c_str();
        }

        void setVertexShaderPath(char const *path)
        {
            vertexShaderPath_.assign(path);
        }

        char const *getFragmentShaderPath() const
        {
            return fragmentShaderPath_.c_str();
        }

        void setFragmentShaderPath(char const *path)
        {
            fragmentShaderPath_.assign(path);
        }

        void create();
        void destroy();

        void invalidate()
        {
            vertexShaderHandle_ = 0;
            fragmentShaderHandle_ = 0;
            programHandle_ = 0;
        }

        void bind()
        {
            glUseProgram(programHandle_);
        }

        void unbind()
        {
            glUseProgram(0);
        }

        GLint getUniformLocation(GLchar const *name)
        {
            return glGetUniformLocation(programHandle_, name);
        }
        
        void setUniform(GLint location, GLint value)
        {
            glUniform1i(location, value);
        }

        void setUniform(GLint location, GLfloat value)
        {
            glUniform1f(location, value);
        }

        template <typename T>
        void setUniform(GLchar const *name, T value)
        {
            GLint location = getUniformLocation(name);
            setUniform(location, value);
        }

        GLuint getHandle() const
        {
            return programHandle_;
        }
        
    private:
        std::string vertexShaderPath_;
        std::string fragmentShaderPath_;

        GLuint vertexShaderHandle_;
        GLuint fragmentShaderHandle_;
        GLuint programHandle_;

        // Noncopyable.
        ShaderProgram(ShaderProgram const &other);
        ShaderProgram &operator=(ShaderProgram const &other);
    };
}

#endif
