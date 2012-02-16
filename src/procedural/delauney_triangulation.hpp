#ifndef CRUST_DELAUNEY_TRIANGULATION_HPP
#define CRUST_DELAUNEY_TRIANGULATION_HPP

#include "geometry.hpp"

#include <vector>
#include <boost/array.hpp>

namespace crust {
    class DelauneyTriangulation {
    public:
        explicit DelauneyTriangulation(Box2 box);

        typedef boost::array<int, 3> IndexArray;

        void addVertex(Vector2 vertex);

        int getVertexCount() const
        {
            return int(vertices_.size());
        }

        Vector2 getVertex(int index) const
        {
            return vertices_[index];
        }

        int getTriangleCount() const
        {
            return int(triangles_.size());
        }

        IndexArray getTriangleIndices(int index) const
        {
            return triangles_[index];
        }
        
        Triangle2 getTriangle(int index) const
        {
            Vector2 v1 = vertices_[triangles_[index][0]];
            Vector2 v2 = vertices_[triangles_[index][1]];
            Vector2 v3 = vertices_[triangles_[index][2]];
            return Triangle2(v1, v2, v3);
        }

        void draw();
        
    private:
        typedef std::vector<Vector2> VertexVector;
        typedef std::vector<IndexArray> TriangleVector;

        VertexVector vertices_;
        TriangleVector triangles_;

        void drawVertices();
        void drawTriangles();
        void drawCircumcircles();
        void drawCircle(Circle2 circle);
    };
}

#endif
