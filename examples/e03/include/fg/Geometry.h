#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
#include "delaunator/delaunator.h"
#include "DualMesh.h"
namespace fog::examples::e03
{
    

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
    
};