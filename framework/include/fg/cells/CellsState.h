/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/Actor.h"
#include "fg/core/CoreMod.h"
#include "fg/MeshBuild.h"

#include "fg/util/Property.h"
#include "fg/core/ManualState.h"
#include "fg/util/DiamondSquare.h"
#include "fg/Common.h"
#include "fg/cells/CellsTerrains.h"
#include "fg/cells/CellsGenerator.h"
#include "fg/Config.h"
#include "fg/Terrains.h"
#include "fg/util/Box2.h"
#include "CellsDatas.h"
namespace fog
{
    using namespace Ogre;

    struct TheTerrains : public Terrains
    {
        struct Options
        {
            int tlsWidth;
            int tlsHeight;
            int terWidth;
            int terHeight;
            INJECT(Options(CellsTerrains::Options opts, Config *config)) : tlsWidth(config->TILES_RANGE.getWidth()), tlsHeight(config->TILES_RANGE.getHeight()),
                                                                           terWidth(opts.width), terHeight(opts.height)
            {
            }
        };
        int tlsWidth;
        int tlsHeight;
        int terWidth;
        int terHeight;
        std::vector<std::vector<CellsVertex>> vertexs;
        Config* config;
        INJECT(TheTerrains(Options opts,Config* config)) : tlsWidth(opts.tlsWidth), tlsHeight(opts.tlsHeight),
                                            terWidth(opts.terWidth), terHeight(opts.terHeight),
                                            config(config),
                                            vertexs(opts.terWidth, std::vector<CellsVertex>(opts.terHeight, CellsVertex()))
        {
        }
        Vector3 getOrigin() override
        {
            return Vector3(0, 0, 0);
        }
        float getHeightWithNormalAtWorldPosition(Vector3 pos, Vector3 *norm) override
        {
            // Vector2 pIn2DV = Context<Node2D>::get()->to2D(pos);

            Vector2 pIn2DV = Point2<float>::from(pos, *Context<Transform::D3_NORMAL_D2>::get());
            return getHeight(pIn2DV);
        }
        float getHeight(Vector2 pIn2DV)
        {

            Point2<float> pIn2D(pIn2DV.x, pIn2DV.y);
            pIn2D = pIn2D / config->CELL_SCALE; //

            // Point2<float> pUV = Cell::getPointInUV(pIn2D, tlsWidth, tlsHeight); // UV
            Point2<float> pUV = pIn2D.transform(Transform::D2CellWorldUV(tlsWidth, tlsHeight));
            Point2<float> p = pUV;
            p.scale(terWidth, terHeight);

            int x = static_cast<int>(std::round(p.x)) % terWidth;
            int y = static_cast<int>(std::round(p.y)) % terHeight;

            if (x < 0)
            {
                x = x + terWidth;
                x = 0;
            }
            if (y < 0)
            {
                y = y + terHeight;
                y = 0;
            }
            float ret = vertexs[x][y].height * config->HEIGHT_SCALE;
            if (config->DEBUG_COUT)
            {
                std::cout << fmt::format(":[{:>.2f},{:>.2f}],[{:>3},{:>3}].h={:>3.1f}", pUV.x, pUV.y, x, y, ret) << std::endl;
            }
            return ret;
        }
    };

    //
    class CellsState : public ManualState
    {

    public:
        struct Options
        {
            std::string texName;
            INJECT(Options(Config *config)) : texName(config->FOG_OF_WAR_TEX_NAME)
            {
            }
        };
        TheTerrains *tts;
        std::vector<std::vector<CellData>> &tiles;
        CellsTerrains *terrains;
        Options options;
        Config* config;

    public:
        INJECT(CellsState(CellsDatas *cDatas, TheTerrains *tts, Options options,
                          CellsTerrains *terrains,
                          Config * config,
                          CoreMod *core)) : ManualState(core), terrains(terrains), tiles(cDatas->tiles),
                                            options(options),
                                            config(config),
                                            tts(tts)
        {
            this->material = "Tiles";
        }
        void init() override
        {
            ManualState::init();
        }
        void rebuildMesh() override
        {
            // CellsTerrains *terrains = Context<CellsTerrains>::get();

            // mesh
            terrains->buildVertexs(tiles, tts->vertexs);

            auto heightFunc = [this](Vector3 &pos, Vector3 *norm)
            {
                return this->tts->getHeightWithNormalAtWorldPosition(pos, norm);
            };
            // Context<Plane>::get()->height = heightFunc; // replace the height func.

            Context<Transform::D2H2D3>::get()->setHeight([this](float x, float y)
                                                         { return this->tts->getHeight(Vector2(x, y)); } //
            );

            // material
            MaterialPtr mat = MaterialManager::getSingletonPtr()->getByName("Tiles");
            // tex0
            std::string texName0 = "TerrainsTex001";
            // Context<CellsTerrains>::get()->createWorldTexture(texName0, tts->vertexs);
            terrains->createWorldTexture(texName0, tts->vertexs);

            mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(texName0);
            mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureFiltering(Ogre::TFO_NONE);
            // tex9

            // std::string texName9 = Context<FogOfWar>::get()->getTexName();
            // std::string texName9 = this->fogOfWar->getTexName();
            std::string texName9 = options.texName;
            mat->getTechnique(0)->getPass(0)->getTextureUnitState(9)->setTextureName(texName9);
            mat->getTechnique(0)->getPass(0)->getTextureUnitState(9)->setTextureFiltering(Ogre::TFO_BILINEAR);

            GpuProgramParametersSharedPtr vParams = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
            vParams->setNamedConstant("tlsWidthInNum", config->TILES_RANGE.getWidth());
            vParams->setNamedConstant("tlsHeightInNum", config->TILES_RANGE.getHeight());

            int step = config->TILE_TERRAIN_QUALITY / config->TILE_MESH_QUALITY;

            int qWidth = terrains->width / step;
            int qHeight = terrains->height / step;

            obj->clear();
            obj->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST);

            int baseIdx = obj->getCurrentVertexCount();

            std::vector<std::vector<Vector3>> norms(qWidth, std::vector<Vector3>(qHeight, Vector3::UNIT_Y));
            std::vector<std::vector<Vector3>> positions(qWidth, std::vector<Vector3>(qHeight, Vector3::ZERO));

            // Cell::Center *cc = Context<Cell::Center>::get();

            // collect position.
            for (int y = 0; y < qHeight; y++)
            {
                for (int x = 0; x < qWidth; x++)
                {
                    int qy = y * step;
                    int qx = x * step;
                    CellKey cKey = tts->vertexs[qx][qy].cKey;

                    // HexTile::Key cis = cc->getCell(cKey);

                    // float scale = Context<Node2D>::get()->getScale();
                    // Vector2 tP = Cell::getOrigin2D(cis,scale);

                    // Vector2 qP = tP + tts->vertexs[qx][qy].originInTile * scale;
                    // Point2<float> qP = tP + tts->vertexs[qx][qy].originInTile * scale;
                    //  scale
                    float h = tts->vertexs[qx][qy].height * config->HEIGHT_SCALE;

                    // Vector3 position3 = Context<Node2D>::get()->to3D(qP, 1);
                    // position.y = h;

                    // Vector3 position = qP.transform3(Transform::D2_NORMAL_D3(h));
                    // Vector3 position = ((cis.cast<float>().transform(Transform::CellCentreByKey()) + tts->vertexs[qx][qy].originInTile) * config->CELL_SCALE).transform3(Transform::D2_NORMAL_D3(h));
                    Vector3 position = cKey.transform3(tts->vertexs[qx][qy].originInTile, h);
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
                    Vector4 plane = Math::calculateFaceNormalWithoutNormalize(p1, p2, p3);
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
