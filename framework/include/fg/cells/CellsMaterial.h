/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/core.h"
#include "fg/ogre.h"
#include "fg/util.h"
#include "CellsVertecies.h"
namespace fog
{
    using namespace Ogre;


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

        INJECT(CellsMaterial(
                      CellsVertecies * cvs,
                      FogOfWarTexture *ftexture,
                      Config *config,
                      Options options))
        {
            this->material = "Tiles";

            // material
            MaterialPtr mat = MaterialManager::getSingletonPtr()->getByName("Tiles");
            // tex0
            std::string texName0 = "TerrainsTex001";
            // Context<CellsTerrains>::get()->createWorldTexture(texName0, tts->vertexs);
            cvs->createWorldTexture(texName0, cvs->vertexs); // TODO texture create by a manager.

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
