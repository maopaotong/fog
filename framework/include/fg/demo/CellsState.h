/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/ogre.h"
#include "fg/util.h"
#include "fg/cells.h"
#include "fg/cells/TransformD2D3.h"
namespace fog
{
    //
    struct CellsState : public Actor
    {

    public:
        Config *config;
        Transforms *tfs;
        TransformD2TD3::Args tfos;

    public:
        INJECT(CellsState(CellsDatas *cDatas,
                          CellsGrids *cvs,
                          CellsMaterial *cMaterial,
                          Transforms *tfs,
                          Config *config,
                          CoreMod *core, Ogre::SceneNode *sceNode, TransformD2TD3::Args tfos)) : Actor(sceNode),
                                                                                                 config(config),
                                                                                                 tfs(tfs),
                                                                                                 tfos(tfos)
        {

            Ogre::ManualObject *obj = core->createManualObject();
            // obj->setQueryFlags();
            sceNode->attachObject(obj);

            int cols = cvs->opts.gridsCols;
            int rows = cvs->opts.gridsRows;

            obj->clear();
            obj->begin(MaterialNames::materialNameForCells, Ogre::RenderOperation::OT_TRIANGLE_LIST);

            int baseIdx = obj->getCurrentVertexCount();

            std::vector<std::vector<Vector3>> norms(cols, std::vector<Vector3>(rows, Ogre::Vector3::UNIT_Y));
            std::vector<std::vector<Vector3>> positions(cols, std::vector<Vector3>(rows, Ogre::Vector3::ZERO));

            // Cell::Center *cc = Context<Cell::Center>::get();

            // collect position.
            for (int y = 0; y < rows; y++)
            {
                for (int x = 0; x < cols; x++)
                {

                    float h = cvs->grids[x][y].aHeight * tfos.heightScale;

                    // Vector3 position = tfs->transform3(CellTransform::transform<Cell::Offset, Cell::Centre>(cKey), cvs->grids[qx][qy].originInCell, h, *tfs->d2hd3);
                    Vector3 position = tfs->transform3(cvs->grids[x][y].a, h); //
                    // position.y = h;
                    positions[x][y] = position;
                }
            }
            // calculate norms

            for (int y = 0; y < rows; y++)
            {
                for (int x = 0; x < cols; x++)
                {
                    Vector3 p1 = positions[x][y];
                    obj->position(p1);
                    Vector3 normNs = calculateNorm(positions, x, y, cols, rows);
                    // normNs = {0,1,0};
                    obj->normal(normNs);

                    obj->textureCoord(p1.x, -p1.z);
                } // end of for
            } // end of for

            // triangle
            for (int y = 0; y < rows - 1; y++)
            {
                bool evenRow = (y % 2 == 0);
                for (int x = 0; x < cols - 1; x++)
                {

                    int a = y * cols + x;
                    int b = y * cols + (x + 1);
                    int c = (y + 1) * cols + (x + 1);
                    int d = (y + 1) * cols + x;

                    int A = baseIdx + a;
                    int B = baseIdx + b;
                    int C = baseIdx + c;
                    int D = baseIdx + d;

                    //
                    if (evenRow)
                    {

                        obj->triangle(A, B, D);
                        obj->triangle(B, C, D);
                    }
                    else
                    {
                        obj->triangle(A, B, C);
                        obj->triangle(A, C, D);
                    }
                }
            }

            obj->end();
            //
        }

        Vector3 get(std::vector<std::vector<Vector3>> &positions, int x, int y, int cols, int rows)
        {
            if (x < 0)
            {
                x += cols;
            }
            if (y < 0)
            {
                y += rows;
            }
            if (x > cols - 1)
            {
                x -= cols;
            }
            if (y > rows - 1)
            {
                y -= rows;
            }
            return positions[x][y];
        }

        Vector3 calculateNorm(std::vector<std::vector<Vector3>> &positions, int x, int y, int cols, int rows)
        {

            std::array<std::tuple<int, int>, 6> neibers;
            bool evenRow = (y % 2 == 0);
            if (evenRow)
            {

                neibers[0] = {1, 0};
                neibers[1] = {0, 1};
                neibers[2] = {-1, 1};
                neibers[3] = {-1, 0};
                neibers[4] = {-1, -1};
                neibers[5] = {0, -1};
            }
            else
            {
                neibers[0] = {1, 0};
                neibers[1] = {1, 1};
                neibers[2] = {0, 1};
                neibers[3] = {-1, 0};
                neibers[4] = {0, -1};
                neibers[5] = {1, -1};
            }

            // Cell::getNeibers(qx, qy, neibersX, neibersY); // TODO
            //

            std::array<Vector3, 6> neibersP;

            for (int i = 0; i < 6; i++)
            {
                neibersP[i] = get(positions, x + std::get<0>(neibers[i]), y + std::get<1>(neibers[i]), cols, rows);
            }

            Vector3 p0 = positions[x][y];
            if (DEBUG_COUT)
            {

                std::cout << fmt::format("======[{},{}]==================", x, y) << std::endl;
                std::cout << fmt::format("p1:({:>8.1f},{:>8.1f},{:>8.1f})", p0.x, p0.y, p0.z) << std::endl;
            }

            Vector3 normNs = Vector3(0, 0, 0);
            for (int i = 0; i < 6; i++)
            {
                Vector3 p1 = neibersP[i];
                Vector3 p2 = neibersP[(i + 1) % 6];
                Ogre::Vector4 plane = Ogre::Math::calculateFaceNormalWithoutNormalize(p0, p1, p2);
                Vector3 normN(plane.x, plane.y, plane.z);
                normN.normalise();
                normNs += normN;
                if (DEBUG_COUT)
                {
                    std::cout << fmt::format("neibers   [{}]-----------------------------------------", i) << std::endl;
                    std::cout << fmt::format("neibers   [{:>3}]p0:({:>8.1f},{:>8.1f},{:>8.1f})", i, p0.x, p0.y, p0.z) << std::endl;
                    std::cout << fmt::format("neibers   [{:>3}]p1:({:>8.1f},{:>8.1f},{:>8.1f})", i, p1.x, p1.y, p1.z) << std::endl;
                    std::cout << fmt::format("neibers   [{:>3}]p2:({:>8.1f},{:>8.1f},{:>8.1f})", i, p2.x, p2.y, p2.z) << std::endl;
                    std::cout << fmt::format("normN     [{:>3}]  :({:>8.1f},{:>8.1f},{:>8.1f})", i, normN.x, normN.y, normN.z) << std::endl;
                    std::cout << fmt::format("normNsRaw [{:>3}]  :({:>8.1f},{:>8.1f},{:>8.1f})", i, normNs.x, normNs.y, normNs.z) << std::endl;
                }
            }
            normNs.normalise();
            if (DEBUG_COUT)
            {
                std::cout << fmt::format("normNsNormalised [{},{}]({:>8.1f},{:>8.1f},{:>8.1f})", x, y, normNs.x, normNs.y, normNs.z) << std::endl;
            }

            return normNs;
        } // end method.
    }; // end class
}; // end of namespace
