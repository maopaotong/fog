/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/core.h"
#include "fg/ogre.h"
#include "fg/util.h"
#include "Terrains.h"
#include "CellsTerrains.h"
#include "CellsDatas.h"
namespace fog
{
    using namespace Ogre;

    struct TheTerrains2
    {
        CellsTerrains *terrains;
        Config *config;
        TheTerrains *tts;
        INJECT(TheTerrains2(CellsTerrains *terrains, TheTerrains *tts, Config *config))
            : terrains(terrains),
              tts(tts),
              config(config)
        {
            // mesh
            // terrains->buildVertexs(tiles, tts->vertexs);

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

}; // end of namespace
