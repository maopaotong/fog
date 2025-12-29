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
            //obj->setQueryFlags();
            sceNode->attachObject(obj);
            int step = tfos.cellsTerrainAmp; // / config->cellsMeshQuality;

            int qWidth = cvs->opts.terWidth / step;
            int qHeight = cvs->opts.terHeight / step;

            obj->clear();
            obj->begin(MaterialNames::materialNameForCells, Ogre::RenderOperation::OT_TRIANGLE_LIST);

            int baseIdx = obj->getCurrentVertexCount();

            std::vector<std::vector<Vector3>> norms(qWidth, std::vector<Vector3>(qHeight, Ogre::Vector3::UNIT_Y));
            std::vector<std::vector<Vector3>> positions(qWidth, std::vector<Vector3>(qHeight, Ogre::Vector3::ZERO));

            // Cell::Center *cc = Context<Cell::Center>::get();

            // collect position.
            for (int y = 0; y < qHeight; y++)
            {
                for (int x = 0; x < qWidth; x++)
                {
                    int qy = y * step;
                    int qx = x * step;
                    CellKey cKey = cvs->grids[qx][qy].cKey;

                    // HexTile::Key cis = cc->getCell(cKey);

                    // float scale = Context<Node2D>::get()->getScale();
                    // Vector2 tP = Cell::getOrigin2D(cis,scale);

                    // Vector2 qP = tP + tts->grids[qx][qy].originInTile * scale;
                    // Point2<float> qP = tP + tts->grids[qx][qy].originInTile * scale;
                    //  scale
                    float h = cvs->grids[qx][qy].height * tfos.heightScale;

                    // Vector3 position3 = Context<Node2D>::get()->to3D(qP, 1);
                    // position.y = h;

                    // Vector3 position = qP.transform3(Transform::D2_NORMAL_D3(h));
                    // Vector3 position = ((cis.cast<float>().transform(Transform::CellCentreByKey()) + tts->grids[qx][qy].originInTile) * config->CELL_SCALE).transform3(Transform::D2_NORMAL_D3(h));
                    // Vector3 position = CellKey::transform3(cKey, cvs->grids[qx][qy].originInTile, h, *config->transformFromD2HToD3Ptr);

                    //Vector3 position = CellKey::transform3(cKey, cvs->grids[qx][qy].originInCell, h, *tfs->d2hd3);

                    Vector3 position = tfs->transform3(CellsTransform::Transform<Cell::PointyTop>::transform<Cell::Offset, Cell::Centre>(cKey), cvs->grids[qx][qy].originInCell, h, *tfs->d2hd3);
                    // position.y = h;

                    positions[x][y] = position;
                }
            }
            // calculate norms

            for (int y = 0; y < qHeight; y++)
            {
                for (int x = 0; x < qWidth; x++)
                {
                    Vector3 p1 = positions[x][y];
                    obj->position(p1);
                    Vector3 normNs = calculateNorm(positions, x, y, qWidth, qHeight);
                    obj->normal(normNs);
                    obj->textureCoord(p1.x, -p1.z);
                } // end of for
            } // end of for

            // triangle
            for (int y = 0; y < qHeight - 1; y++)
            {
                for (int x = 0; x < qWidth - 1; x++)
                {

                    int a = y * qWidth + x;
                    int b = y * qWidth + (x + 1);
                    int c = (y + 1) * qWidth + (x + 1);
                    int d = (y + 1) * qWidth + x;

                    int ba = baseIdx + a;
                    int bb = baseIdx + b;
                    int bc = baseIdx + c;
                    int bd = baseIdx + d;

                    //

                    obj->triangle(ba, bb, bc);

                    obj->triangle(ba, bc, bd);
                }
            }

            obj->end();
            //
        }

        Vector3 calculateNorm(std::vector<std::vector<Vector3>> &positions, int qx, int qy, int qWidth, int qHeight)
        {
            const int N = 4;
            // South,East,North,West
            int neibersX[N] = {qx, qx + 1, qx, qx - 1};
            int neibersY[N] = {qy - 1, qy, qy + 1, qy};

            // Cell::getNeibers(qx, qy, neibersX, neibersY); // TODO
            //

            Vector3 neibersP[N];
            int neibersCount = 0;
            for (int i = 0; i < N; i++)
            {
                int nX = neibersX[i];
                int nY = neibersY[i];

                if (nX >= 0 && nX < qWidth && nY >= 0 && nY < qHeight) // TODO
                {
                    neibersP[neibersCount] = positions[nX][nY];
                    neibersCount++;
                }
            }

            Vector3 normNs; //
            Vector3 p1 = positions[qx][qy];
            if (DEBUG_COUT)
            {

                std::cout << fmt::format("======[{},{}]==================", qx, qy) << std::endl;
                std::cout << fmt::format("p1:({:>8.1f},{:>8.1f},{:>8.1f})", p1.x, p1.y, p1.z) << std::endl;
                std::cout << fmt::format("neibersCount:{}", neibersCount) << std::endl;
            }
            if (neibersCount == N)
            {
                normNs = Vector3(0, 0, 0);
                for (int i = 0; i < neibersCount; i++)
                {
                    Vector3 p2 = neibersP[i];
                    Vector3 p3 = neibersP[(i + 1) % neibersCount];
                    Ogre::Vector4 plane = Ogre::Math::calculateFaceNormalWithoutNormalize(p1, p2, p3);
                    Vector3 normN(plane.x, plane.y, plane.z);
                    normN.normalise();
                    normNs += normN;
                    if (DEBUG_COUT)
                    {
                        std::cout << fmt::format("neibers   [{}]-----------------------------------------", i) << std::endl;
                        std::cout << fmt::format("neibers   [{:>3}]p1:({:>8.1f},{:>8.1f},{:>8.1f})", i, p1.x, p1.y, p1.z) << std::endl;
                        std::cout << fmt::format("neibers   [{:>3}]p2:({:>8.1f},{:>8.1f},{:>8.1f})", i, p2.x, p2.y, p2.z) << std::endl;
                        std::cout << fmt::format("neibers   [{:>3}]p3:({:>8.1f},{:>8.1f},{:>8.1f})", i, p3.x, p3.y, p3.z) << std::endl;
                        std::cout << fmt::format("normN     [{:>3}]  :({:>8.1f},{:>8.1f},{:>8.1f})", i, normN.x, normN.y, normN.z) << std::endl;
                        std::cout << fmt::format("normNsRaw [{:>3}]  :({:>8.1f},{:>8.1f},{:>8.1f})", i, normNs.x, normNs.y, normNs.z) << std::endl;
                    }
                }
                normNs.normalise();
                if (DEBUG_COUT)
                {
                    std::cout << fmt::format("normNsNormalised [{},{}]({:>8.1f},{:>8.1f},{:>8.1f})", qx, qy, normNs.x, normNs.y, normNs.z) << std::endl;
                }
            }
            else
            {
                normNs = Vector3(0, 1, 0);
            }
            return normNs;
        } // end method.
    }; // end class
}; // end of namespace
