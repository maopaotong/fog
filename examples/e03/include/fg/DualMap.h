#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
#include "delaunator/delaunator.h"
#include "DualMesh.h"
namespace fog::examples::e03
{
    using namespace delaunator;
    struct DualMap
    {
        

        DualMesh &mesh;
        std::vector<float> elevation_t;
        std::vector<float> elevation_r;
        DualMap(DualMesh &mesh) : mesh(mesh), elevation_r(mesh.numRegions, 0.0f), elevation_t(mesh.numSolidTriangles, 0.0f)
        {
        }
    };

};