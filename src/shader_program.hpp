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

        void createVertexShader();
        void createFragmentShader();
        void createProgram();
    };
}

#endif
