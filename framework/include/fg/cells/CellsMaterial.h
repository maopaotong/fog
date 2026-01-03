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
#include "MaterialNames.h"
#include "TransformD2D3.h"
namespace fog
{

    struct CellsMaterial
    {
        struct Args
        {
            std::string texName;
            bool debugWireFrame;
            INJECT(Args(Config *config)) : texName(config->FOG_OF_WAR_TEX_NAME), debugWireFrame(config->DEBUG_polygonMode_wireFrame)

            {
            }
        };
        Args opts;
        CellsGridsGenerator::Args ggopts;
        INJECT(CellsMaterial(
            CellsGrids *cvs,
            FogOfWarTexture *ftexture,
            WorldTexGenerator *wtg,
            Config *config,
            CellsDatas::Args &cdos,
            TransformD2TD3::Args tfos,
            CellsGridsGenerator::Args ggopts,
            Args opts)) : opts(opts), ggopts(ggopts)
        {
            // material
            Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(MaterialNames::materialNameForCells);
            // tex0
            std::string texName001 = "TerrainsTex001";
            std::string texName010 = "TerrainsTex010";
            // Context<CellsTerrains>::get()->createWorldTexture(texName0, tts->vertexs);
            // cvs->createWorldTexture(texName0, cvs->grids); // TODO texture create by a manager.
            wtg->createWorlTextures(texName001, texName010);

            Ogre::Pass *pass = mat->getTechnique(0)->getPass(0);
            // text 01
            pass->getTextureUnitState(0)->setTextureName(texName001);
            pass->getTextureUnitState(0)->setTextureFiltering(Ogre::TFO_NONE);
            // text 02
            pass->getTextureUnitState(10)->setTextureName(texName010);
            pass->getTextureUnitState(10)->setTextureFiltering(Ogre::TFO_NONE);

            // tex9

            // std::string texName9 = Context<FogOfWar>::get()->getTexName();
            // std::string texName9 = this->fogOfWar->getTexName();
            std::string texName9 = ftexture->opts.texName;
            pass->getTextureUnitState(9)->setTextureName(texName9);
            pass->getTextureUnitState(9)->setTextureFiltering(Ogre::TFO_BILINEAR);

            // arguments.
            Ogre::GpuProgramParametersSharedPtr vParams = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
            vParams->setNamedConstant("cellsCols", cdos.cellsRange.getWidth());
            vParams->setNamedConstant("cellsRows", cdos.cellsRange.getHeight());
            vParams->setNamedConstant("meshQuality", tfos.cellsMeshQuality);
            vParams->setNamedConstant("unitCols", ggopts.unitCols);
            vParams->setNamedConstant("unitRows", ggopts.unitRows);
            vParams->setNamedConstant("unitWidth", Cell::LayoutInfo<CellLayout>::unitWidth);
            vParams->setNamedConstant("unitHeight", Cell::LayoutInfo<CellLayout>::unitHeight);

            if (opts.debugWireFrame)
            {
                pass->setPolygonMode(Ogre::PM_WIREFRAME);
            }
        }
    };

}; // end of namespace
