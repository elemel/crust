#include "debug_draw.hpp"

#include <cmath>
#include <SDL/SDL_opengl.h>

namespace crust {
    void debugDrawLine(float x1, float y1, float x2, float y2)
    {
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }
    
    void debugDrawCircle(float x, float y, float radius, int vertexCount)
    {
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < vertexCount; ++i) {
            float angle = 2.0f * M_PI * float(i) / float(vertexCount);
            glVertex2f(x + radius * std::cos(angle),
                       y + radius * std::sin(angle));
        }
        glEnd();
    }
}
