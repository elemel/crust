#ifndef CRUST_DEBUG_DRAW_HPP
#define CRUST_DEBUG_DRAW_HPP

namespace crust {
    void debugDrawLine(float x1, float y1, float x2, float y2);
    void debugDrawCircle(float x, float y, float radius, int vertexCount = 16);
}

#endif
