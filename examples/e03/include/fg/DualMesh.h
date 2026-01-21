#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
#include "delaunator/delaunator.h"

namespace fog::examples::e03
{
    using namespace delaunator;

    struct DualMesh
    {
        struct Data
        {
            std::vector<double> points; // x,y coords
            int numBoundaryPoints;
            Delaunator delaunator;
            Data(std::vector<double> points, int numBoundaryPoints) : points(points),
                                                                      numBoundaryPoints(numBoundaryPoints),
                                                                      delaunator(this->points)
            {
            }
        };

        static unsigned int t_from_s(unsigned int s) { return s / 3; }
        static unsigned int s_next_s(unsigned int s) { return (s % 3 == 2) ? s - 2 : s + 1; }

        std::vector<double> _vertex_r;
        std::vector<std::size_t> _triangles; // actually the sides of triangles, size = 3 * real_triangles_size.
        std::vector<std::size_t> _halfedges;
        std::vector<std::array<double, 2>> _vertex_t;
        unsigned int numSides;
        unsigned int numRegions;
        unsigned int numTriangles;
        unsigned int numSolidSides;
        unsigned int numSolidTriangles;

        std::vector<unsigned int> _s_of_r;

        DualMesh(Data &data) : _vertex_r(data.points),
                               _triangles(data.delaunator.triangles),
                               _halfedges(data.delaunator.halfedges),
                               numRegions(data.points.size() / 2),
                               numSides(data.delaunator.triangles.size()),      // numSolidSides + ghost_sides.
                               numSolidSides(data.delaunator.triangles.size()), //
                               numTriangles(data.delaunator.triangles.size() / 3),
                               _vertex_t(data.delaunator.triangles.size() / 3, std::array<double, 2>{2, 0.0}),
                               numSolidTriangles(data.delaunator.triangles.size() / 3)
        {

            // Construct an index for finding sides connected to a region
            this->_s_of_r = std::vector<unsigned int>(numRegions);
            for (int s = 0; s < _triangles.size(); s++)
            {
                unsigned int endpoint = _triangles[DualMesh::s_next_s(s)];
                if (this->_s_of_r[endpoint] == 0 || _halfedges[s] == -1)
                {
                    this->_s_of_r[endpoint] = s;
                }
            }
            //

            for (int i = 0; i < numTriangles; i++)
            {
                int aI = _triangles[i * 3 + 0];
                int bI = _triangles[i * 3 + 1];
                int cI = _triangles[i * 3 + 2];

                double aX = _vertex_r[_triangles[aI] * 2 + 0];
                double aY = _vertex_r[_triangles[aI] * 2 + 1];
                double bX = _vertex_r[_triangles[bI] * 2 + 0];
                double bY = _vertex_r[_triangles[bI] * 2 + 1];
                double cX = _vertex_r[_triangles[cI] * 2 + 0];
                double cY = _vertex_r[_triangles[cI] * 2 + 1];

                _vertex_t[i][0] = (aX + bX + cX) / 3;
                _vertex_t[i][1] = (aY + bY + cY) / 3;
            }
        }

        double x_of_r(int r) { return this->_vertex_r[r * 2 + 0]; }
        double y_of_r(int r) { return this->_vertex_r[r * 2 + 1]; }
        double x_of_t(int t) { return this->_vertex_t[t][0]; }
        double y_of_t(int t) { return this->_vertex_t[t][1]; }

        unsigned int r_begin_s(int s) { return this->_triangles[s]; }
        unsigned int r_end_s(int s) { return this->_triangles[DualMesh::s_next_s(s)]; }

        unsigned int t_inner_s(int s) { return DualMesh::t_from_s(s); }
        unsigned int t_outer_s(int s) { return DualMesh::t_from_s(this->_halfedges[s]); }

        unsigned int s_opposite_s(int s) { return this->_halfedges[s]; }
    };

};