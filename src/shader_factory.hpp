#ifndef CRUST_SHADER_COMPILER_HPP
#define CRUST_SHADER_COMPILER_HPP

#include <vector>
#include <SDL/SDL_opengl.h>

namespace crust {
    class ShaderFactory {
    public:
        GLuint compileShader(GLenum type, char const *file);
        GLuint linkProgram(GLuint shader1, GLuint shader2 = 0, GLuint shader3 = 0);

    private:
        std::vector<char> source_;

        GLchar const *loadSource(char const *file);
    };
}

#endif
