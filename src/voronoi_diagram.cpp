#include "voronoi_diagram.hpp"

#include "delauney_triangulation.hpp"

#include <map>
#include <set>
#include <SDL/SDL_opengl.h>

namespace crust {
    Polygon2 VoronoiDiagram::getPolygon(int index)
    {
        Polygon2 result;
        IndexVector const &indices = polygons_[index];
        for (std::size_t i = 0; i < indices.size(); ++i) {
            result.vertices.push_back(vertices_[indices[i]]);
        }
        return result;
    }

    void VoronoiDiagram::generate(DelauneyTriangulation const &source)
    {
        typedef std::map<int, IndexVector> TriangleMap;

        // Clear old state.
        vertices_.clear();
        polygons_.clear();

        // Store the circumcenter of each source triangle.
        for (int i = 0; i < source.getTriangleCount(); ++i) {
            Triangle2 triangle = source.getTriangle(i);
            Vector2 circumcenter = triangle.getCircumcircle().center;
            vertices_.push_back(circumcenter);
        }

        // Map each source vertex to its adjacent source triangles.
        TriangleMap triangles;
        for (int i = 0; i < source.getTriangleCount(); ++i) {
            DelauneyTriangulation::IndexArray indices =
                source.getTriangleIndices(i);
            triangles[indices[0]].push_back(i);
            triangles[indices[1]].push_back(i);
            triangles[indices[2]].push_back(i);
        }

        // Generate a polygon for each source vertex.
        for (TriangleMap::iterator i = triangles.begin(); i != triangles.end();
             ++i)
        {
            typedef std::multimap<float, int> SortedIndexMap;

            Vector2 vertex = source.getVertex(i->first);
            IndexVector const &triangleIndices = i->second;
            
            // Sort the circumcenters by angle relative to the source vertex.
            SortedIndexMap sortedIndices;
            for (IndexVector::const_iterator j = triangleIndices.begin();
                 j != triangleIndices.end(); ++j)
            {
                Vector2 diff = vertices_[*j] - vertex;
                float angle = std::atan2(diff.y, diff.x);
                sortedIndices.insert(std::make_pair(angle, *j));
            }

            // Create polygon.
            IndexVector polygonIndices;
            for (SortedIndexMap::iterator i = sortedIndices.begin();
                 i != sortedIndices.end(); ++i)
            {
                polygonIndices.push_back(i->second);
            }
            polygons_.push_back(polygonIndices);
        }
    }

    void VoronoiDiagram::draw()
    {
        for (std::size_t i = 0; i < polygons_.size(); ++i) {
            IndexVector const &indices = polygons_[i];
            glBegin(GL_LINE_LOOP);
            for (std::size_t j = 0; j < indices.size(); ++j) {
                Vector2 vertex = vertices_[indices[j]];
                glVertex2f(vertex.x, vertex.y);
            }
            glEnd();
        }
    }
}
