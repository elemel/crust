#include "delauney_triangulation.hpp"

#include <cmath>
#include <map>
#include <set>
#include <SDL/SDL_opengl.h>

namespace crust {
    DelauneyTriangulation::DelauneyTriangulation(Box2 box)
    {
        vertices_.push_back(box.p1);
        vertices_.push_back(Vector2(box.p2.x, box.p1.y));
        vertices_.push_back(box.p2);
        vertices_.push_back(Vector2(box.p1.x, box.p2.y));
        IndexArray indices1 = { { 0, 1, 2 } };
        triangles_.push_back(indices1);
        IndexArray indices2 = { { 0, 2, 3 } };
        triangles_.push_back(indices2);
    }

    // http://en.wikipedia.org/wiki/Bowyer-Watson_algorithm
    void DelauneyTriangulation::addVertex(Vector2 vertex)    
    {
        // Add vertex.
        vertices_.push_back(vertex);

        // Remove the triangles that have circumcircles that contain the new
        // vertex. Remember the indices of the removed triangles.
        std::set<int> uniqueIndices;
        for (int i = 0; i < int(triangles_.size()); ++i) {
            if (getTriangle(i).getCircumcircleDeterminant(vertex) > 0.0f) {
                uniqueIndices.insert(triangles_[i][0]);
                uniqueIndices.insert(triangles_[i][1]);
                uniqueIndices.insert(triangles_[i][2]);
                std::swap(triangles_[i], triangles_.back());
                triangles_.pop_back();
                --i;
            }
        }

        // Sort the indices by angle relative to the new vertex.
        std::multimap<float, int> sortedIndices;
        for (std::set<int>::iterator i = uniqueIndices.begin();
             i != uniqueIndices.end(); ++i)
        {
            Vector2 diff = getVertex(*i) - vertex;
            float angle = std::atan2(diff.y, diff.x);
            sortedIndices.insert(std::make_pair(angle, *i));
        }

        // Create new triangles from the new vertex and the sorted indices.
        for (std::multimap<float, int>::iterator i = sortedIndices.begin();
             i != sortedIndices.end(); ++i)
        {
            std::multimap<float, int>::iterator j = i;
            ++j;
            if (j == sortedIndices.end()) {
                j = sortedIndices.begin();
            }
            int v1 = int(vertices_.size()) - 1;
            int v2 = i->second;
            int v3 = j->second;
            IndexArray triangle = { { v1, v2, v3 } };
            triangles_.push_back(triangle);
        }
    }

    void DelauneyTriangulation::draw()
    {
        drawVertices();
        drawTriangles();
        // drawCircumcircles();
    }

    void DelauneyTriangulation::drawVertices()
    {
        glBegin(GL_POINTS);
        for (std::size_t i = 0; i < vertices_.size(); ++i) {
            glVertex2f(vertices_[i].x, vertices_[i].y);
        }
        glEnd();
    }

    void DelauneyTriangulation::drawTriangles()
    {
        glBegin(GL_LINES);
        for (std::size_t i = 0; i < triangles_.size(); ++i) {
            Triangle2 triangle = getTriangle(i);

            glVertex2f(triangle.v1.x, triangle.v1.y);
            glVertex2f(triangle.v2.x, triangle.v2.y);

            glVertex2f(triangle.v2.x, triangle.v2.y);
            glVertex2f(triangle.v3.x, triangle.v3.y);

            glVertex2f(triangle.v3.x, triangle.v3.y);
            glVertex2f(triangle.v1.x, triangle.v1.y);
        }
        glEnd();
    }

    void DelauneyTriangulation::drawCircumcircles()
    {
        for (std::size_t i = 0; i < triangles_.size(); ++i) {
            drawCircle(getTriangle(i).getCircumcircle());
        }
    }

    void DelauneyTriangulation::drawCircle(Circle2 circle)
    {
        int vertexCount = 64;
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < vertexCount; ++i) {
            float angle = 2.0f * M_PI * float(i) / float(vertexCount);
            float x = circle.center.x + circle.radius * std::cos(angle);
            float y = circle.center.y + circle.radius * std::sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
    }
}
