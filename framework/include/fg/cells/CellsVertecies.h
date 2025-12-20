/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util.h"
#include "CellsDatas.h"
#include "CellsVertex.h"

namespace fog
{

    struct CellsVertecies
    {
        struct Options
        {
            int tlsWidth;
            int tlsHeight;
            int terWidth;
            int terHeight;
            int quality;
            float heightAmpOfHill;
            float heightAmpOfMountain;
            float hillDistribution;
            float mountainPeakDistribution;
            INJECT(Options(Config *config)) : tlsWidth(config->cellsRange.getWidth()),
                                              quality(config->cellsTerrainQuality),
                                              tlsHeight(config->cellsRange.getHeight()),
                                              heightAmpOfHill(config->heightAmpOfHill),
                                              heightAmpOfMountain(config->heightAmpOfMountain),
                                              hillDistribution(config->hillPeakDistribution),
                                              mountainPeakDistribution(config->mountainPeakDistribution)

            {

                this->terWidth = tlsWidth * quality;                          //
                this->terHeight = tlsHeight * quality * std::sqrt(3.0) / 2.0; // based on the toploy of cells.
            }
        };

        Config *config;
        std::vector<std::vector<CellsVertex>> vertexs;
        int tWidth;
        int tHeight;
        int width;
        int height;
        float rectWidth;
        float rectHeight;
        float rectRad;
        float heightAmpOfHill;
        float heightAmpOfMountain;
        std::vector<std::vector<CellData>> tiles;

        Options opts;

        INJECT(CellsVertecies(Options opts, CellsDatas *cDatas,
                              Config *config)) : opts(opts),
                                                 vertexs(opts.terWidth,
                                                         std::vector<CellsVertex>(opts.terHeight, CellsVertex())),
                                                 config(config),
                                                 heightAmpOfHill(opts.heightAmpOfHill),
                                                 heightAmpOfMountain(opts.heightAmpOfMountain)
        {

            tWidth = opts.tlsWidth;
            tHeight = opts.tlsHeight;
            auto &tiles = cDatas->cells;
            auto &hMap = vertexs;
            width = opts.terWidth;
            height = opts.terHeight;
            rectWidth = 2.0 / opts.quality; // rad of tile = 1 , width of tile = 2;
            rectHeight = rectWidth;         // rect height == width
            this->rectRad = (rectHeight + rectWidth) / 2.0;
            // float rectWidth = static_cast<float>(tWidth) * 2.0f / static_cast<float>(width);
            // float rectHeight = static_cast<float>(tHeight) * 2.0f / static_cast<float>(height) * std::sqrt(3) / 2.0f;
            //
            std::vector<std::vector<CellsVertex *>> centreRectMap(tWidth, std::vector<CellsVertex *>(tHeight, nullptr));
            // resove the terrain height of the centre rect for each tile.

            makeHeight(hMap, cDatas, centreRectMap);
            //makeMountainRange(CellTypes::MOUNTAIN, hMap, cDatas, centreRectMap);
            makeMountainRange(CellTypes::HILL, hMap, cDatas, centreRectMap);
        }
        struct
        {
        };
        //
        // Connect peaks of mountain.
        void makeMountainRange(CellType type, std::vector<std::vector<CellsVertex>> &hMap, CellsDatas *cDatas, std::vector<std::vector<CellsVertex *>> &centreRectMap)
        {
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    CellsVertex &vert = hMap[x][y]; // centre of rect.
                    int regions = vert.getRegions();
                    if (vert.types[0] != type || vert.isPeak || regions > 1) //
                    {
                        continue;
                    }
                    // is moutain rect and not the peak, so the height can be changed.
                    // collect neibers peaks
                    int rad = opts.quality * 2;
                    int px1 = x - rad;
                    int py1 = y - rad;
                    int px2 = x + rad;
                    int py2 = y + rad;
                    float sumH = 0.0f;
                    float sumW = 0.0f;
                    int peaks = 0;
                    Point2<float> centre(x, y);

                    for (int px = px1; px < px2; px++)
                    {
                        for (int py = py1; py < py2; py++)
                        {

                            if ((px >= 0 && px < width && py >= 0 && py < height) && (hMap[px][py].types[0] == type && hMap[px][py].isPeak))
                            {

                                float distance = centre.distance(Point2<float>(px, py));
                                float weight = 1.0f / (distance * distance * distance);
                                sumH += hMap[px][py].height * weight;
                                sumW += weight;
                                peaks++;
                            }
                        }
                    }

                    if (peaks > 0)
                    {
                        hMap[x][y].height = sumH / sumW;
                    }
                    //
                }
            }
        }

        void makeHeight(std::vector<std::vector<CellsVertex>> &hMap, CellsDatas *cDatas, std::vector<std::vector<CellsVertex *>> &centreRectMap)
        {

            std::mt19937 randGen(23665289);
            std::bernoulli_distribution randHill(this->opts.hillDistribution); //
            std::uniform_real_distribution<float> randHeightOfHill(0.0f, 0.5f);

            /*
            int n2 = 1;
            int width2 = 1;
            while (width2 < std::max(width, height))
            {
                width2 = std::pow(2, n2++) + 1;
            }

            std::vector<std::vector<float>> hMap2(width2, std::vector<float>(width2, 0.0f));
            DiamondSquare::generateAndNormalise(hMap2, width2, config->generatorRoughness2, config->seedOfGenerator2);
            */

            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {

                    auto &cells = cDatas->cells;

                    float centreX = static_cast<float>(x) * rectWidth;
                    float centreY = static_cast<float>(y) * rectHeight;
                    Vector2 points[5];
                    points[0] = Vector2(centreX, centreY);                                        // centre point of the rect.
                    points[1] = Vector2(centreX + rectWidth / 2.0f, centreY - rectHeight / 2.0f); //
                    points[2] = Vector2(centreX + rectWidth / 2.0f, centreY + rectHeight / 2.0f); //
                    points[3] = Vector2(centreX - rectWidth / 2.0f, centreY + rectHeight / 2.0f); //
                    points[4] = Vector2(centreX - rectWidth / 2.0f, centreY - rectHeight / 2.0f); //

                    CellKey cKeys[5]; // find the 5 point in which cell.
                    for (int i = 0; i < 5; i++)
                    {
                        cKeys[i] = Point2<float>(points[i].x, points[i].y).transform(Transform::CentreToCellKey());
                        cKeys[i].x = std::clamp<int>(cKeys[i].x, 0, tWidth - 1);
                        cKeys[i].y = std::clamp<int>(cKeys[i].y, 0, tHeight - 1);
                    }

                    CellData &cell0 = cells[cKeys[0].x][cKeys[0].y];
                    // tile centre position.
                    // Vector2 tileCentreP = Cell::getOrigin2D(cKeys[0].x, cKeys[0].y);
                    Vector2 tileCentreP = cKeys[0].getCentre();
                    //
                    hMap[x][y].cKey = cKeys[0]; // centre cell.
                    hMap[x][y].originInCell = points[0] - tileCentreP;
                    hMap[x][y].types[0] = cell0.type;
                    // set corner's type

                    std::unordered_set<CellType> typeSet; // totol types of 5 cells.
                    CellType type0 = cell0.type;
                    for (int i = 1; i < 5; i++) // check other 4 corner's type. normally the max different types is 3, include the centre.
                    {
                        CellData &tlI = cells[cKeys[i].x][cKeys[i].y];
                        CellType typeI = tlI.type;
                        if (typeI != type0)
                        {
                            typeSet.insert(typeI);
                        }
                    }

                    if (typeSet.size() == 0)
                    {
                        hMap[x][y].types[1] = type0;
                        hMap[x][y].types[2] = type0;
                    }
                    else if (typeSet.size() == 1)
                    {
                        hMap[x][y].types[1] = type0;
                        hMap[x][y].types[2] = *typeSet.begin();
                    }
                    else if (typeSet.size() == 2)
                    {
                        auto it = typeSet.begin();
                        hMap[x][y].types[1] = *it++;
                        hMap[x][y].types[2] = *it++;
                    }
                    else if (typeSet.size() == 3)
                    {
                        auto it = typeSet.begin();
                        hMap[x][y].types[1] = *it++;
                        hMap[x][y].types[2] = *it++;
                        // TODO what's up? span on the world edge?
                    }
                    else
                    { // impossbile?bug?
                        throw std::runtime_error("the rect is tool big?");
                    }

                    float typeHeight = cellTypeHeight(cell0);
                    float ht = 0.0f;

                    // tile's centre is in this rect.
                    if (Rect::isPointInSide(tileCentreP, points[0], rectWidth, rectHeight)) //
                    {                                                                       // is the center rect of the tile.

                        // remember the centre rect for each tile.
                        centreRectMap[cKeys[0].x][cKeys[0].y] = &hMap[x][y];
                    }

                    int regions = hMap[x][y].getRegions();

                    if (regions == 1) // region inner.
                    {
                        if (cell0.type == CellTypes::HILL)
                        {
                            // hill is random offset + type height.
                            if (randHill(randGen))
                            {
                                ht = typeHeight * (1 + (this->opts.heightAmpOfHill - 1) * randHeightOfHill(randGen));
                                hMap[x][y].isPeak = true;
                            }
                            else
                            {
                                ht = typeHeight;
                            }
                        }
                        else if (cell0.type == CellTypes::MOUNTAIN)
                        {
                            if (randHill(randGen))
                            {
                                ht = typeHeight * (1 + (this->opts.heightAmpOfMountain - 1) * randHeightOfHill(randGen));
                                hMap[x][y].isPeak = true;
                            }
                            else
                            {
                                ht = typeHeight;
                            }
                        }
                        else
                        {
                            ht = typeHeight;
                        }
                    }
                    else // regions = 2 or 3
                    {
                        // calculate height of the region edge by samples.
                        const static Transform::CentreToCellKey C2CK;
                        // only for the edge of regions.
                        ht = calculateRectHeightBySamples(points[0], [this, &cells](float x, float y)
                                                          {
                                                              // translate point in the rect to the nearest cell key?
                                                              // the cKey must be one of the 3 cell types calculated above.
                                                              CellKey cKey = Point2<float>(x, y).transform(C2CK);
                                                              int tx = std::clamp<int>(cKey.x, 0, tWidth - 1);
                                                              int ty = std::clamp<int>(cKey.y, 0, tHeight - 1);

                                                              CellData &ttl = cells[tx][ty];
                                                              return cellTypeHeight(ttl); //
                                                          });
                        //
                    } // end of if, height now prepared as a terrains in theory.
                    //

                    hMap[x][y].height = ht;
                }
            }
        }

        float map(float low1, float heigh1, float low2, float high2, float value)
        {
            return low2 + (high2 - low2) * ((value - low1) / (heigh1 - low1));
        }

        /**
         *
         */
        float cellTypeHeight(CellData &tl)
        {

            float tlHeight = 0.0;

            switch (tl.type)
            {
            case (CellTypes::OCEAN):
                tlHeight = config->heightOfOcean;
                break;
            case (CellTypes::SHORE):
            case (CellTypes::LAKE):
                tlHeight = config->heightOfShore;
                break;
            case (CellTypes::PLAIN):
                tlHeight = config->heightOfPlain;
                break;
            case (CellTypes::HILL):
                tlHeight = config->heightOfHill;
                break;
            case (CellTypes::MOUNTAIN):
                tlHeight = config->heightOfMountain;
                break;
            case (CellTypes::FROZEN):
                tlHeight = config->heightOfFrozen;
                break;
            default:
                tlHeight = config->heightOfFrozen;
                break;
            }
            return tlHeight;
        }

        /**
         * Deal with the sub cell which span multiple different type of terrain.
         * Calculate the height by sampling points's avg height.
         */
        float calculateRectHeightBySamples(Ogre::Vector2 center,
                                           std::function<float(float, float)> getHeight)
        {

            const int SAMPLES = 3; // 3x3 = 9 points；
            float totalHeight = 0.0f;
            for (int i = 0; i < SAMPLES; ++i)
            {
                for (int j = 0; j < SAMPLES; ++j)
                {
                    //
                    float u = (i + 0.5f) / SAMPLES; //
                    float v = (j + 0.5f) / SAMPLES;

                    float x = center.x + (u - 0.5f) * rectWidth;
                    float y = center.y + (v - 0.5f) * rectHeight;

                    // HexTile::Key k = Cell::getCellKey(p, 1); //

                    totalHeight += getHeight(x, y);
                }
            }

            return totalHeight / (SAMPLES * SAMPLES);
        }

        struct WorldTexOp
        {

            std::vector<std::vector<CellsVertex>> &hMap;
            int typePlot[11] = {0}; // for debug.
            int width;
            int height;
            unsigned char *data;
            Config *config;
            WorldTexOp(std::vector<std::vector<CellsVertex>> &hMap, int w, int h, Config *config) : hMap(hMap), width(w), height(h), config(config)
            {
                data = new unsigned char[w * h * 4];
            }
            ~WorldTexOp()
            {
                delete[] data;
            }

            void operator()()
            {

                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {

                        Box2<int> debugRange = config->debugPrintTerrainsTexRange;
                        CellsVertex &v = hMap[x][y];
                        if (v.types[0] < 10)
                        {
                            typePlot[v.types[0]]++;
                        }
                        else
                        {
                            typePlot[10]++;
                        }

                        int idx = (y * width + x) * 4;

                        // R as the type of the centre point of the rect, the precision is based on the qulity parameter configred.

                        data[idx] = v.types[0]; /** 0 .. 15 **/
                        // G 2rd type of the rect.
                        data[idx + 1] = v.types[1];
                        // B 3rd type of the rect.
                        data[idx + 2] = v.types[2];
                        // A
                        data[idx + 3] = v.distanceToEdge(1.0) * 100; //

                        if (debugRange.isIn(x, y))
                        {
                            std::cout << fmt::format("texure[{:>2},{:>2}]:({:>3}|{:>3}|{:>3}|{:>3})", x, y, data[idx], data[idx + 1], data[idx + 2], data[idx + 3]) << std::endl;
                        }
                    }
                }
            }
        };

        // TODO create texture by a texture manager.
        // TODO as well as the FogOfWar Texture creation.
        // World texture is used as the meta data for the shader to determine the child texture.
        void createWorldTexture(std::string name, std::vector<std::vector<CellsVertex>> &hMap)
        {
            WorldTexOp texOp(hMap, width, height, config);
            texOp();
            Ogre::TexturePtr texture = TextureFactory::createTexture(name, width, height);

            TextureFactory::updateTexture(texture, width, height, texOp.data);

            for (int i = 0; i < 11; i++)
            {
                std::cout << fmt::format("typePlot[{}] is {:>3}", i, texOp.typePlot[i]) << std::endl;
            }
        }
    };

}; // end of namespace
