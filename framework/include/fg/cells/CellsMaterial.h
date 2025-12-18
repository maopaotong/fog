/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/core.h"
#include "fg/ogre.h"
#include "fg/util.h"

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
            INJECT(Options(CellsTerrains::Options opts, Config *config)) : tlsWidth(config->cellsRange.getWidth()), tlsHeight(config->cellsRange.getHeight()),
                                                                           terWidth(opts.width), terHeight(opts.height)
            {
            }
        };
        int tlsWidth;
        int tlsHeight;
        int terWidth;
        int terHeight;
        std::vector<std::vector<CellsVertex>> vertexs;
        Config *config;
        INJECT(TheTerrains(Options opts, Config *config)) : tlsWidth(opts.tlsWidth), tlsHeight(opts.tlsHeight),
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

            Vector2 pIn2DV = Point2<float>::from(pos, *config->transformD3NormalToD2Ptr);
            return getHeight(pIn2DV);
        }
        float getHeight(Vector2 pIn2DV)
        {

            Point2<float> pIn2D(pIn2DV.x, pIn2DV.y);
            pIn2D = pIn2D / config->cellScale; //

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
            float ret = vertexs[x][y].height * config->heightScale;
            if (config->debugCout)
            {
                std::cout << fmt::format(":[{:>.2f},{:>.2f}],[{:>3},{:>3}].h={:>3.1f}", pUV.x, pUV.y, x, y, ret) << std::endl;
            }
            return ret;
        }
    };

    struct TheTerrains2
    {
        TheTerrains *tts;
        std::vector<std::vector<CellData>> &tiles;
        CellsTerrains *terrains;
        Config *config;
        INJECT(TheTerrains2(CellsDatas *cDatas,
                            TheTerrains *tts,
                            Config *config,
                            CellsTerrains *terrains))
            : terrains(terrains),
              config(config),
              tiles(cDatas->tiles),
              tts(tts)
        {
            // mesh
            terrains->buildVertexs(tiles, tts->vertexs);

            auto heightFunc = [this](Vector3 &pos, Vector3 *norm)
            {
                return this->tts->getHeightWithNormalAtWorldPosition(pos, norm);
            };
            // Context<Plane>::get()->height = heightFunc; // replace the height func.
            // TODO move to other place.
            config->transformFromD2HToD3Ptr->setHeight([this](float x, float y)
                                                    { return this->tts->getHeight(Vector2(x, y)); } //
            );
        }
    };

    struct CellsMaterial
    {
        struct Options
        {
            std::string texName;
            INJECT(Options(Config *config)) : texName(config->FOG_OF_WAR_TEX_NAME)
            {
            }
        };
        std::string material;

        INJECT(CellsMaterial(CellsTerrains *terrains,
                      TheTerrains *tts,
                      FogOfWarTexture *ftexture,
                      TheTerrains2 *tts2,
                      Config *config,
                      Options options))
        {
            this->material = "Tiles";

            // material
            MaterialPtr mat = MaterialManager::getSingletonPtr()->getByName("Tiles");
            // tex0
            std::string texName0 = "TerrainsTex001";
            // Context<CellsTerrains>::get()->createWorldTexture(texName0, tts->vertexs);
            terrains->createWorldTexture(texName0, tts->vertexs); // TODO texture create by a manager.

            mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(texName0);
            mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureFiltering(Ogre::TFO_NONE);
            // tex9

            // std::string texName9 = Context<FogOfWar>::get()->getTexName();
            // std::string texName9 = this->fogOfWar->getTexName();
            std::string texName9 = ftexture->opts.texName;
            mat->getTechnique(0)->getPass(0)->getTextureUnitState(9)->setTextureName(texName9);
            mat->getTechnique(0)->getPass(0)->getTextureUnitState(9)->setTextureFiltering(Ogre::TFO_BILINEAR);

            GpuProgramParametersSharedPtr vParams = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
            vParams->setNamedConstant("tlsWidthInNum", config->cellsRange.getWidth());
            vParams->setNamedConstant("tlsHeightInNum", config->cellsRange.getHeight());
        }
    };

}; // end of namespace
