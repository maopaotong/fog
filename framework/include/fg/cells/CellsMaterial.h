/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>

#include "fg/ogre.h"
#include "fg/util.h"
#include "CellsGrids.h"
#include "WorldTexGenerator.h"

namespace fog
{
    using namespace Ogre;

    struct CellsMaterial
    {
        struct Options
        {
            std::string texName;
            bool debugWireFrame;
            INJECT(Options(Config *config)) : texName(config->FOG_OF_WAR_TEX_NAME), debugWireFrame(config->DEBUG_polygonMode_wireFrame)
            {
            }
        };
        std::string material;
        Options opts;
        INJECT(CellsMaterial(
            CellsGrids *cvs,
            FogOfWarTexture *ftexture,
            WorldTexGenerator * wtg,
            Config *config,
            Options opts)) : opts(opts)
        {
            this->material = "Tiles";

            // material
            MaterialPtr mat = MaterialManager::getSingletonPtr()->getByName("Tiles");
            // tex0
            std::string texName001 = "TerrainsTex001";
            std::string texName010 = "TerrainsTex010";
            // Context<CellsTerrains>::get()->createWorldTexture(texName0, tts->vertexs);
            //cvs->createWorldTexture(texName0, cvs->grids); // TODO texture create by a manager.
            wtg->createWorlTextures(texName001, texName010);

            Pass *pass = mat->getTechnique(0)->getPass(0);
            //text 01
            pass->getTextureUnitState(0)->setTextureName(texName001);
            pass->getTextureUnitState(0)->setTextureFiltering(Ogre::TFO_NONE);
            //text 02
            pass->getTextureUnitState(10)->setTextureName(texName010);
            pass->getTextureUnitState(10)->setTextureFiltering(Ogre::TFO_NONE);
            
            // tex9

            // std::string texName9 = Context<FogOfWar>::get()->getTexName();
            // std::string texName9 = this->fogOfWar->getTexName();
            std::string texName9 = ftexture->opts.texName;
            pass->getTextureUnitState(9)->setTextureName(texName9);
            pass->getTextureUnitState(9)->setTextureFiltering(Ogre::TFO_BILINEAR);

            //arguments.
            GpuProgramParametersSharedPtr vParams = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
            vParams->setNamedConstant("tlsWidthInNum", config->cellsRange.getWidth());
            vParams->setNamedConstant("tlsHeightInNum", config->cellsRange.getHeight());
            if (opts.debugWireFrame)
            {
                pass->setPolygonMode(PM_WIREFRAME);
            }
        }
    };

}; // end of namespace
