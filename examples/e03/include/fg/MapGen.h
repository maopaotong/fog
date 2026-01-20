#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
#include "delaunator/delaunator.h"

namespace fog::examples::e03
{
    using namespace delaunator;
    static std::vector<double> toFlatPoints(std::vector<PoissonDisk::Point> points)
    {
        std::vector<double> ps2;
        for (PoissonDisk::Point p : points)
        {
            ps2.push_back(p.x);
            ps2.push_back(p.y);
        }
        return ps2;
    }

    static std::vector<PoissonDisk::Point> generateInteriorBoundaryPoints(PoissonDisk::Bounds bounds, double boundarySpacing)
    {
        // https://www.redblobgames.com/x/2314-poisson-with-boundary/
        const double epsilon = 1e-4;
        const double curvature = 1.0;
        int W = std::ceil((bounds.width - 2 * curvature) / boundarySpacing);
        int32_t H = std::ceil((bounds.height - 2 * curvature) / boundarySpacing);
        std::vector<PoissonDisk::Point> points;
        // Top and bottom
        for (int q = 0; q < W; q++)
        {
            double t = static_cast<double>(q) / static_cast<double>(W);
            double dx = (bounds.width - 2 * curvature) * t;
            double dy = epsilon + curvature * 4 * std::pow((t - 0.5), 2);
            points.push_back({bounds.x + curvature + dx, bounds.y + dy});
            points.push_back({bounds.x + bounds.width - curvature - dx, bounds.y + bounds.height - dy});
        }
        // Left and right
        for (int r = 0; r < H; r++)
        {
            double t = static_cast<double>(r) / static_cast<double>(H);
            double dy = (bounds.height - 2 * curvature) * t;
            double dx = epsilon + curvature * 4 * std::pow((t - 0.5), 2);
            points.push_back({bounds.x + dx, bounds.y + bounds.height - curvature - dy});
            points.push_back({bounds.x + bounds.width - dx, bounds.y + curvature + dy});
        }
        return points;
    }

    static std::vector<PoissonDisk::Point> generateExteriorBoundaryPoints(PoissonDisk::Bounds bounds, double boundarySpacing)
    {
        // https://www.redblobgames.com/x/2314-poisson-with-boundary/
        const double curvature = 1.0;
        const double diagonal = boundarySpacing / std::sqrt(2);
        std::vector<PoissonDisk::Point> points;

        int W = std::ceil((bounds.width - 2 * curvature) / boundarySpacing);
        int H = std::ceil((bounds.height - 2 * curvature) / boundarySpacing);
        // Top and bottom
        for (int q = 0; q < W; q++)
        {
            double t = static_cast<double>(q) / static_cast<double>(W);
            double dx = (bounds.width - 2 * curvature) * t + boundarySpacing / 2;
            points.push_back({bounds.x + dx, bounds.y - diagonal});
            points.push_back({bounds.x + bounds.width - dx, bounds.y + bounds.height + diagonal});
        }
        // Left and right
        for (int r = 0; r < H; r++)
        {
            int t = static_cast<double>(r) / static_cast<double>(H);
            int dy = (bounds.height - 2 * curvature) * t + boundarySpacing / 2;
            points.push_back({bounds.x - diagonal, bounds.y + bounds.height - dy});
            points.push_back({bounds.x + bounds.width + diagonal, bounds.y + dy});
        }
        // Corners
        points.push_back({bounds.x - diagonal, bounds.y - diagonal});
        points.push_back({bounds.x + bounds.width + diagonal, bounds.y - diagonal});
        points.push_back({bounds.x - diagonal, bounds.y + bounds.height + diagonal});
        points.push_back({bounds.x + bounds.width + diagonal, bounds.y + bounds.height + diagonal});
        return points;
    }

    struct Data
    {
        std::vector<double> points;
        int numBoundaryPoints;
        Delaunator delaunator;
        Data(std::vector<double> points, int numBoundaryPoints) : points(points),
                                                                  numBoundaryPoints(numBoundaryPoints),
                                                                  delaunator(this->points)
        {
        }
    };

    struct DualMesh
    {
        static int t_from_s(int s) { return s / 3; }

        std::vector<double> _vertex_r;
        std::vector<std::size_t> _triangles; // actually the sides of triangles, size = 3 * real_triangles_size.
        std::vector<std::size_t> _halfedges;
        std::vector<std::array<double, 2>> _vertex_t;
        unsigned int numSides;
        unsigned int numRegions;
        unsigned int numTriangles;
        unsigned int numSolidSides;

        DualMesh(Data &data) : _vertex_r(data.points),
                               _triangles(data.delaunator.triangles),
                               _halfedges(data.delaunator.halfedges),
                               numRegions(data.points.size() / 2),
                               numSides(data.delaunator.triangles.size()),      // numSolidSides + ghost_sides.
                               numSolidSides(data.delaunator.triangles.size()), //
                               numTriangles(data.delaunator.triangles.size() / 3),
                               _vertex_t(data.delaunator.triangles.size() / 3, std::array<double, 2>{2, 0.0})
        {
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

        unsigned int s_next_s(int s) { return DualMesh::s_next_s(s); }
        unsigned int s_prev_s(int s) { return DualMesh::s_prev_s(s); }

        unsigned int s_opposite_s(int s) { return this->_halfedges[s]; }
    };
    struct Map
    {
    };

    struct Geometry
    {

        static void setMeshGeometry(DualMesh &mesh, float *vData, unsigned int vSize, int offset)
        {
            int idx = 0;
            for (int r = 0; r < mesh.numRegions; r++)
            {

                vData[idx * vSize + offset + 0] = mesh.x_of_r(r);
                vData[idx * vSize + offset + 1] = mesh.y_of_r(r);
                idx++;
            }
            for (int t = 0; t < mesh.numTriangles; t++)
            {
                vData[idx * vSize + offset + 0] = mesh.x_of_t(t);
                vData[idx * vSize + offset + 1] = mesh.y_of_t(t);
            }
        };

        static void setMapGeometry(DualMesh &mesh, unsigned int *iData)
        {

            float mountain_folds = 0.05;

            int i = 0;
            for (int s = 0; s < mesh.numSolidSides; s++)
            {
                int s_opposite = mesh.s_opposite_s(s);
                int r1 = mesh.r_begin_s(s);
                int r2 = mesh.r_begin_s(s_opposite);
                int t1 = mesh.t_inner_s(s);
                int t2 = mesh.t_inner_s(s_opposite);

                // Each quadrilateral is turned into two triangles, so each
                // half-edge gets turned into one. There are two ways to fold
                // a quadrilateral. This is usually a nuisance but in this
                // case it's a feature. See the explanation here
                // https://www.redblobgames.com/x/1725-procedural-elevation/#rendering
                bool is_valley = false;
                if (is_valley)
                {
                    // It's a coastal or river edge, forming a valley
                    iData[i++] = r1;
                    iData[i++] = mesh.numRegions + t2;
                    iData[i++] = mesh.numRegions + t1;
                }
                else
                {
                    // It's a ridge
                    iData[i++] = r1;
                    iData[i++] = r2;
                    iData[i++] = mesh.numRegions + t1;
                }
            }
        };
    };
    struct MapGen
    {

        static Data setupData()
        {

            double spacing = 5.5;
            double mountainSpacing = 35;
            double boundarySpacing = spacing * std::sqrt(2);
            int mountainRetries = 30;
            int retries = 6;
            unsigned int seed = 12345;

            PoissonDisk::Bounds bounds(0, 0, 100, 100);
            std::vector<PoissonDisk::Point> interiorBoundaryPoints = generateInteriorBoundaryPoints(bounds, boundarySpacing);
            std::vector<PoissonDisk::Point> exteriorBoundaryPoints = generateExteriorBoundaryPoints(bounds, boundarySpacing);

            PoissonDisk::Generator<std::mt19937> mountainPointsGen(bounds, mountainSpacing, mountainRetries, std::mt19937{seed});
            for (auto p : interiorBoundaryPoints)
            {
                if (!mountainPointsGen.add_point(p, spacing * spacing))
                {
                    throw std::runtime_error("failed to add point.");
                }
            }

            std::vector<PoissonDisk::Point> interiorPoints = mountainPointsGen.fill();
            int numMoutainPoints = interiorPoints.size() - interiorBoundaryPoints.size();

            PoissonDisk::Generator<std::mt19937> pointsGen(bounds, spacing, retries, std::mt19937{seed});
            for (auto p : interiorPoints)
            {
                if (!pointsGen.add_point(p))
                {
                    throw std::runtime_error("failed to add point.");
                }
            }

            interiorPoints = pointsGen.fill();

            std::vector<PoissonDisk::Point> points;
            points.insert(points.end(), std::make_move_iterator(exteriorBoundaryPoints.begin()), std::make_move_iterator(exteriorBoundaryPoints.end()));
            points.insert(points.end(), std::make_move_iterator(interiorPoints.begin()), std::make_move_iterator(interiorPoints.end()));

            return Data{toFlatPoints(points), static_cast<int>(exteriorBoundaryPoints.size())};
        }
    };
};