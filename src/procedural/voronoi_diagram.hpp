#ifndef CRUST_VORONOI_DIAGRAM_HPP
#define CRUST_VORONOI_DIAGRAM_HPP

#include "geometry.hpp"

namespace crust {
    class DelauneyTriangulation;

    class VoronoiDiagram {
    public:
        int getVertexCount() const
        {
            return int(vertices_.size());
        }

        Vector2 getVertex(int index) const
        {
            return vertices_[index];
        }
        
        int getPolygonCount() const
        {
            return int(polygons_.size());
        }

        int getPolygonIndexCount(int index) const
        {
            return int(polygons_[index].size());
        }

        int getPolygonIndex(int polygonIndex, int indexIndex) const
        {
            return polygons_[polygonIndex][indexIndex];
        }
        
        Polygon2 getPolygon(int index);

        void generate(DelauneyTriangulation const &source);

        void draw();
        
    private:
        typedef std::vector<Vector2> VertexVector;
        typedef std::vector<int> IndexVector;
        typedef std::vector<IndexVector> PolygonVector;

        VertexVector vertices_;
        PolygonVector polygons_;
    };
}

#endif
