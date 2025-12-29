/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/util.h"
#include "CellsDatas.h"
#include "CellGrid.h"

namespace fog
{

    struct CellsGridsGenerator
    {
        struct Args
        {
            int terWidth;
            int terHeight;
            int quality;
            float heightAmpOfHill;
            float heightAmpOfMountain;
            float hillDistribution;
            float mountainDistribution;
            int hillRad;
            bool makeMountainRange;
            CellsDatas::Args &cells;

            int cellsTerrainAmp;
            int cellsMeshQuality;

            SELFG(Args, "config")
            MEMBERK(cellsTerrainAmp, "CELLS_TERRAIN_AMP")
            MEMBERK(cellsMeshQuality, "TILE_MESH_QUALITY")

            INJECT(Args(Config *config, CellsDatas::Args &cells)) : cells(cells),
                                                                    heightAmpOfHill(config->heightAmpOfHill),
                                                                    heightAmpOfMountain(config->heightAmpOfMountain),
                                                                    hillDistribution(config->hillPeakDistribution),
                                                                    mountainDistribution(config->mountainPeakDistribution),
                                                                    makeMountainRange(config->makeMountainRange)

            {
            }

            INIT(init)()
            {
                quality = cellsTerrainAmp * cellsMeshQuality;
                hillRad = quality;
                this->terWidth = cells.cellsRange.getWidth() * quality;                          //
                this->terHeight = cells.cellsRange.getHeight() * quality * unitHeight / unitWidth; // based on the toploy of cells.
            }
        };

        Config *config;
        int tWidth;
        int tHeight;
        int width;
        int height;
        float rectWidth;
        float rectHeight;
        float rectRad;
        float heightAmpOfHill;
        float heightAmpOfMountain;

        Args opts;

        INJECT(CellsGridsGenerator(Args opts, Config *config)) : opts(opts),
                                                                 config(config),
                                                                 heightAmpOfHill(opts.heightAmpOfHill),
                                                                 heightAmpOfMountain(opts.heightAmpOfMountain)
        {
        }

        void generate(std::vector<std::vector<CellsGrid>> &hMap, CellsDatas *cDatas)
        {

            tWidth = opts.cells.cellsRange.getWidth();
            tHeight = opts.cells.cellsRange.getHeight();
            auto &tiles = cDatas->cells;
            width = opts.terWidth;
            height = opts.terHeight;
            rectWidth = 2.0 / opts.quality; // rad of tile = 1 , width of tile = 2;
            rectHeight = rectWidth;         // rect height == width
            this->rectRad = (rectHeight + rectWidth) / 2.0;
            //
            std::vector<std::vector<CellsGrid *>> centreRectMap(tWidth, std::vector<CellsGrid *>(tHeight, nullptr));
            // resove the terrain height of the centre rect for each tile.

            makeHeight(hMap, cDatas, centreRectMap);
            // makeMountainRange(CellTypes::MOUNTAIN, hMap, cDatas, centreRectMap);
            // makeHillRange(CellTypes::HILL, opts.hillRad, hMap, cDatas);

            if (opts.makeMountainRange)
            {
                MakeMountainRangeOnCellOp(tWidth, tHeight, width, height).makeMountainRangeOnCell(hMap, [](CellsGrid &cv)
                                                                                                  { return cv.types[0] == CellTypes::MOUNTAIN || cv.types[0] == CellTypes::FRZ_MOUNTAIN; });
            }
        }

        void tryMakePeak(float height, std::vector<std::vector<CellsGrid>> &hMap, int x, int y, int w, int h)
        {
            if (!Iteration::isValidRectIndex(x, y, w, h))
            {
                return;
            }
            hMap[x][y].isPeak = true;
            hMap[x][y].height = height;
            hMap[x][y].userData = 1;
        }

        int makeMountainRangeOnRegion(std::unordered_set<CellKey, CellKey::Hash> &skips, int loops, CellType type, int rad, std::vector<std::vector<CellsGrid>> &hMap, CellsDatas *cDatas, std::vector<std::vector<CellsGrid *>> &centreRectMap)
        {
            int hits = 0;
            std::mt19937 randGen(23669983);
            float randHeightLow = 0.0;
            float randHeightHigh = 1.0;
            std::uniform_real_distribution<float> randHeight(randHeightLow, randHeightHigh);
            int totalHit = 0;
            for (int tx = 0; tx < tWidth; tx++)
            {
                for (int ty = 0; ty < tHeight; ty++)
                {
                    CellKey cKey(tx, ty);
                    if (skips.find(cKey) != skips.end())
                    {
                        continue;
                    }

                    // TODO connect peak between cells in same region.
                    // iterate the edge in same region.
                }
            }
            return hits;
        }

        struct MakeMountainRangeOnCellOp
        {
            int tWidth;
            int tHeight;
            int width;
            int height;
            MakeMountainRangeOnCellOp(int tW, int tH, int w, int h) : tWidth(tW), tHeight(tH), width(w), height(h)
            {
            }

            Box2<int> cellOuterBox(CellKey cKey)
            {
                Box2<float> box =CellsGroup::getOuterBoxInUV(cKey, tWidth, tHeight); // cover the entire tile.
                box.scale(width, height);
                return box.cast<int>();
            }
            template <typename F>
            void makeMountainRangeOnCell(std::vector<std::vector<CellsGrid>> &hMap,
                                         F &&typeFunc)
            {
                std::mt19937 randGen(23669983);
                std::uniform_real_distribution<float> rand01(0, 1);
                for (int tx = 0; tx < tWidth; tx++)
                {
                    for (int ty = 0; ty < tHeight; ty++)
                    {
                        CellKey cKey(tx, ty);
                        // if (skips.find(cKey) != skips.end())
                        // {
                        //     continue;
                        // }

                        std::vector<Point2<int>> peaks;

                        Box2<int> box = cellOuterBox(cKey);

                        for (int x = box.p1.x; x < box.p2.x; x++)
                        {
                            for (int y = box.p1.y; y < box.p2.y; y++)
                            {
                                if (!Iteration::isValidRectIndex(x, y, width, height))
                                {
                                    continue;
                                }
                                CellsGrid &thePV = hMap[x][y];
                                if (typeFunc(thePV) && thePV.isPeak)
                                {
                                    peaks.push_back(Point2<int>(x, y));
                                }
                            }
                        }
                        if (peaks.size() < 2)
                        {
                            continue;
                        }

                        // Find the 2 rect with max distance, make mountains at the line betwen the 2 rect.
                        int maxDI = 0;
                        int maxDJ = 0;
                        float maxDistance = 0;
                        for (int i = 0; i < peaks.size(); i++)
                        {
                            for (int j = 0; j < peaks.size(); j++)
                            {
                                float dis = peaks[i].distance(peaks[j]);

                                if (dis > maxDistance)
                                {
                                    maxDI = i;
                                    maxDJ = j;
                                    maxDistance = dis;
                                }
                            }
                        }
                        // makeMidAsPeak(peaks[maxDI], peaks[maxDJ], hMap, randGen, randHeight);
                        //  randomly pick two peak to connect.

                        std::unordered_set<int> processed;
                        int fails = 0;
                        int success = 0;
                        while (success < peaks.size() && fails < peaks.size())
                        {

                            int a = static_cast<int>(peaks.size() * rand01(randGen)) % peaks.size();
                            int b = static_cast<int>(peaks.size() * rand01(randGen)) % peaks.size();
                            if (a == b || processed.count(a) > 0 || processed.count(b) > 0)
                            {
                                fails++;
                                continue;
                            }

                            success += 2;
                            processed.insert(a);
                            processed.insert(b);
                            makeMidAsPeak(peaks[a], peaks[b], hMap, randGen, rand01);
                        }
                    }
                }
            } // end make range

            void makeMidAsPeak(Point2<int> p1, Point2<int> p2, std::vector<std::vector<CellsGrid>> &hMap,
                               std::mt19937 &randGen, std::uniform_real_distribution<float> &rand01)
            {
                CellType type = hMap[p1.x][p1.y].types[0];

                if (hMap[p2.x][p2.y].types[0] != type)
                {
                    return;
                }

                Point2<int> midP = (p1 + p2) / 2;
                if (midP == p1 || midP == p2)
                {
                    Point2<int> midP1 = Point2<int>(p1.x, p2.y);
                    Point2<int> midP2 = Point2<int>(p2.x, p1.y);
                    doTryMakePeak(midP1, p1, p2, hMap);
                    doTryMakePeak(midP2, p1, p2, hMap);
                    return;
                }

                //
                doTryMakePeak(midP, p1, p2, hMap);

                makeMidAsPeak(p1, midP, hMap, randGen, rand01);
                makeMidAsPeak(p2, midP, hMap, randGen, rand01);
            }

            void doTryMakePeak(Point2<int> midP, Point2<int> p1, Point2<int> p2, std::vector<std::vector<CellsGrid>> &hMap)
            {
                CellsGrid &midPV = hMap[midP.x][midP.y];

                if (midPV.isPeak)
                {
                    return;
                }
                CellsGrid &p1PV = hMap[p1.x][p1.y];
                CellsGrid &p2PV = hMap[p2.x][p2.y];

                midPV.height = (p1PV.height + p2PV.height) / 2.0;
                midPV.isPeak = true;
            }
        };

        //
        // Connect peaks of hills by make the rect height .
        void makeHillRange(CellType type, int rad, std::vector<std::vector<CellsGrid>> &hMap, CellsDatas *cDatas)
        {
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    CellsGrid &vert = hMap[x][y]; // centre of rect.
                    int regions = vert.getRegions();
                    if (vert.types[0] != type || vert.isPeak || regions > 1) //
                    {
                        continue;
                    }
                    // is moutain rect and not the peak, so the height can be changed.
                    // collect neibers peaks

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
                        hMap[x][y].isPeak = true; // modified height meaning a peak.
                    }
                    //
                }
            }
        }

        void makeHeight(std::vector<std::vector<CellsGrid>> &hMap, CellsDatas *cDatas, std::vector<std::vector<CellsGrid *>> &centreRectMap)
        {

            std::mt19937 randGen(23665289);
            std::bernoulli_distribution randHill(this->opts.hillDistribution);         //
            std::bernoulli_distribution randMountain(this->opts.mountainDistribution); //
            std::uniform_real_distribution<float> randHeightH(-0.1f, 0.9f);
            std::uniform_real_distribution<float> randHeightM(0.0f, 1.0f);

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
                    // amplify the rect helps to make the boder jugment at shader program more easy.
                    // But, this kind of solution for some reason does now work well.
                    // The root cause is the that the uv cords geting the wrong cell/type value from texture few border rects.
                    // Even set the amp to 2 it is still have mis-match and wrong cell for points on border.
                    // Thus we should find another way to deal with this mismatch on the boder of regions.
                    // No need to process the non-border cells for rendering.
                    // Option 1. Mix the color or some other rendering operation on the border of regions.
                    // Option 2. Finger out the boder rect; and convert the near border but inner-region rects into border rect.
                    // Border rect is charactered by:
                    // 1. Span multiple regions .
                    // 2. In another word, two or three types and cells related to it.
                    // So we should wilden the border line and get more cells involed for texture generation for shader's using.

                    float amp = 1;
                    // 0th is the centre point of the rect.
                    points[0] = Vector2(centreX, centreY);
                    points[1] = Vector2(centreX + amp * rectWidth / 2.0f, centreY - amp * rectHeight / 2.0f); //
                    points[2] = Vector2(centreX + amp * rectWidth / 2.0f, centreY + amp * rectHeight / 2.0f); //
                    points[3] = Vector2(centreX - amp * rectWidth / 2.0f, centreY + amp * rectHeight / 2.0f); //
                    points[4] = Vector2(centreX - amp * rectWidth / 2.0f, centreY - amp * rectHeight / 2.0f); //

                    CellKey cKeys[5]; // find the 5 point in which cell.
                    for (int i = 0; i < 5; i++)
                    {
                        // cKeys[i] = Point2<float>(points[i].x, points[i].y).transform(Transform::CentreToCellKey());
                        cKeys[i] = CellTransform::transform<Cell::Centre,Cell::Offset>(points[i]);
                        cKeys[i].x = std::clamp<int>(cKeys[i].x, 0, tWidth - 1);
                        cKeys[i].y = std::clamp<int>(cKeys[i].y, 0, tHeight - 1);
                    }

                    CellData &cell0 = cells[cKeys[0].x][cKeys[0].y];
                    // tile centre position.
                    // Vector2 tileCentreP = Cell::getOrigin2D(cKeys[0].x, cKeys[0].y);
                    Vector2 tileCentreP = CellTransform::transform<Cell::Offset,Cell::Centre>(cKeys[0]);
                    //
                    hMap[x][y].cKey = cKeys[0]; // centre cell.
                    hMap[x][y].originInCell = points[0] - tileCentreP;
                    //
                    hMap[x][y].types[0] = cell0.type;
                    hMap[x][y].cKeys[0] = cKeys[0];
                    // set corner's type

                    std::unordered_set<CellKey, CellKey::Hash> keySet; // totol types of 5 cells.
                    for (int i = 1; i < 5; i++)                                        // check other 4 corner's type. normally the max different types is 3, include the centre.
                    {
                        if (cKeys[i] != cKeys[0])
                        {
                            keySet.insert(cKeys[i]);
                        }
                    }

                    if (keySet.size() == 0) // all rect inside the same cell.
                    {
                        hMap[x][y].cKeys[1] = cKeys[0];
                        hMap[x][y].cKeys[2] = cKeys[0];
                    }
                    else if (keySet.size() == 1)
                    {
                        hMap[x][y].cKeys[1] = *keySet.begin();
                        hMap[x][y].cKeys[2] = cKeys[0];
                    }
                    else if (keySet.size() == 2)
                    {
                        auto it = keySet.begin();
                        hMap[x][y].cKeys[1] = *it++;
                        hMap[x][y].cKeys[2] = *it++;
                    }
                    else if (keySet.size() == 3)
                    {
                        auto it = keySet.begin();
                        hMap[x][y].cKeys[1] = *it++;
                        hMap[x][y].cKeys[2] = *it++;
                        // TODO what's up? span on the world edge?
                    }
                    else
                    { // impossbile?bug?
                        throw std::runtime_error("the rect is tool big?");
                    }
                    //
                    for (int i = 1; i < 3; i++)
                    {
                        CellKey cKey = hMap[x][y].cKeys[i];
                        hMap[x][y].types[i] = cells[cKey.x][cKey.y].type;
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
                                ht = typeHeight + this->opts.heightAmpOfHill * randHeightH(randGen);
                                hMap[x][y].isPeak = true;
                            }
                            else
                            {
                                ht = typeHeight;
                            }
                        }
                        else if (cell0.type == CellTypes::MOUNTAIN || cell0.type == CellTypes::FRZ_MOUNTAIN)
                        {
                            if (randMountain(randGen))
                            {
                                ht = typeHeight + this->opts.heightAmpOfMountain * randHeightM(randGen);
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
                        // const static Transform::CentreToCellKey C2CK;
                        // only for the edge of regions.
                        ht = calculateRectHeightBySamples(points[0], [this, &cells](float x, float y)
                                                          {
                                                              // translate point in the rect to the nearest cell key?
                                                              // the cKey must be one of the 3 cell types calculated above.
                                                              // CellKey::Offset cKey = Point2<float>(x, y).transform(C2CK);
                                                              CellKey cKey = CellTransform::transform<Cell::Centre,Cell::Offset>(Point2<float>(x, y));

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

        static float map(float low1, float heigh1, float low2, float high2, float value)
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
            case (CellTypes::FRZ_SHORE):
            case (CellTypes::LAKE):
                tlHeight = config->heightOfShore;
                break;
            case (CellTypes::PLAIN):
            case (CellTypes::FRZ_PLAIN):
                tlHeight = config->heightOfPlain;
                break;
            case (CellTypes::HILL):
                tlHeight = config->heightOfHill;
                break;
            case (CellTypes::MOUNTAIN):
            case (CellTypes::FRZ_MOUNTAIN):

                tlHeight = config->heightOfMountain;
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
    };

}; // end of namespace
