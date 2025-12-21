/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "CellsDatas.h"
#include "CellsVertex.h"
#include "CellsVertecies.h"

namespace fog
{
    using namespace Ogre;

    struct TransformD2TD3
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
                int quality = config->getTerrainQuality();
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
        float scale;
        INJECT(TransformD2TD3(Options opts, Config *config, CellsVertecies *cvs)) : tlsWidth(opts.tlsWidth), tlsHeight(opts.tlsHeight),
                                                                                   terWidth(opts.terWidth), terHeight(opts.terHeight),
                                                                                   config(config),
                                                                                   cvs(cvs),
                                                                                   scale(config->cellScale)
        {
        }

        void operator()(float &x, float &y, float &z)
        {
            x *= scale;
            y *= scale;
            z = -y;
            y = getHeight(x, y);
        }

        float getHeight(float px, float py)
        {

            Point2<float> pIn2D(px, py);
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
