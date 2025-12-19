/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <vector>
#include "fg/core.h"
#include "fg/ogre.h"
#include "fg/util.h"
#include "CellsDatas.h"
#include "CellsVertex.h"
#include "CellsVertecies.h"
namespace fog
{
    using namespace Ogre;

    struct TransformD2HToD3
    {
        struct Options
        {
            int tlsWidth;
            int tlsHeight;
            int terWidth;
            int terHeight;
            INJECT(Options(Config *config)) : tlsWidth(config->cellsRange.getWidth()),
                                              tlsHeight(config->cellsRange.getHeight())

            {
                int quality = config->cellsTerrainQuality;
                this->terWidth = tlsWidth * quality;                          //
                this->terHeight = tlsHeight * quality * std::sqrt(3.0) / 2.0; // based on the toploy of cells.
            }
        };
        int tlsWidth;
        int tlsHeight;
        int terWidth;
        int terHeight;

        CellsVertecies *cvs;
        Config *config;
        INJECT(TransformD2HToD3(Options opts, Config *config, CellsVertecies *cvs)) : tlsWidth(opts.tlsWidth), tlsHeight(opts.tlsHeight),
                                                                                 terWidth(opts.terWidth), terHeight(opts.terHeight),
                                                                                 config(config),
                                                                                 cvs(cvs)
        {
            config->transformFromD2HToD3Ptr->setHeight([this](float x, float y)
                                                       {
                                                           return this->getHeight(Vector2(x, y)); //
                                                       } //
            );
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
            float ret = cvs->vertexs[x][y].height * config->heightScale;
            if (config->debugCout)
            {
                std::cout << fmt::format(":[{:>.2f},{:>.2f}],[{:>3},{:>3}].h={:>3.1f}", pUV.x, pUV.y, x, y, ret) << std::endl;
            }
            return ret;
        }
    };

}; // end of namespace
