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
            INJECT(Options(Config *config)) : tlsWidth(config->cellsRange.getWidth()),
                                              quality(config->cellsTerrainQuality),
                                              tlsHeight(config->cellsRange.getHeight())

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
        INJECT(CellsVertecies(Options opts, CellsDatas *cDatas, Config *config)) : vertexs(opts.terWidth,                                                                                           
                                                                                           std::vector<CellsVertex>(opts.terHeight, CellsVertex())),
                                                                                           config(config)
        {

            tWidth = opts.tlsWidth;
            tHeight = opts.tlsHeight;
            auto &tiles = cDatas->tiles;
            auto &hMap = vertexs;
            width = opts.terWidth;
            height = opts.terHeight;
            rectWidth = 2.0 / opts.quality; // rad of tile = 1 , width of tile = 2;
            rectHeight = rectWidth;         // rect height == width
            this->rectRad = (rectHeight + rectWidth) / 2.0;
            // float rectWidth = static_cast<float>(tWidth) * 2.0f / static_cast<float>(width);
            // float rectHeight = static_cast<float>(tHeight) * 2.0f / static_cast<float>(height) * std::sqrt(3) / 2.0f;
            //
            std::vector<std::vector<CellsVertex *>> tileCentreMap(tWidth, std::vector<CellsVertex *>(tHeight, nullptr));
            // resove the terrain height of the centre rect for each tile.
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    float centreX = static_cast<float>(x) * rectWidth;
                    float centreY = static_cast<float>(y) * rectHeight;
                    Vector2 points[5];
                    points[0] = Vector2(centreX, centreY);
                    points[1] = Vector2(centreX, centreY - rectHeight / 2.0f); //
                    points[2] = Vector2(centreX + rectWidth / 2.0f, centreY);  //
                    points[3] = Vector2(centreX, centreY + rectHeight / 2.0f); //
                    points[4] = Vector2(centreX - rectWidth / 2.0f, centreY);  //

                    CellKey cKeys[5]; //
                    for (int i = 0; i < 5; i++)
                    {

                        // cKeys[i] = Cell::getCellKey(points[i], 1.0); //
                        cKeys[i] = Point2<float>(points[i].x, points[i].y).transform(Transform::CentreToCellKey());
                        cKeys[i].x = std::clamp<int>(cKeys[i].x, 0, tWidth - 1);
                        cKeys[i].y = std::clamp<int>(cKeys[i].y, 0, tHeight - 1);
                    }

                    CellData &tl0 = tiles[cKeys[0].x][cKeys[0].y];
                    // tile centre position.
                    // Vector2 tileCentreP = Cell::getOrigin2D(cKeys[0].x, cKeys[0].y);
                    Vector2 tileCentreP = cKeys[0].getCentre();
                    //
                    hMap[x][y].cKey = cKeys[0]; // centre cell.
                    hMap[x][y].originInTile = points[0] - tileCentreP;
                    hMap[x][y].types[0] = tl0.type;
                    // set corner's type

                    std::unordered_set<CellType> typeSet;
                    CellType type0 = tl0.type;
                    for (int i = 1; i < 5; i++) // check other 4 corner's type. normally the max different types is 3, include the centre.
                    {
                        CellData &tlI = tiles[cKeys[i].x][cKeys[i].y];
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

                    float typeHeight = defineTileHeight(tl0);
                    // tile's centre is in this rect.
                    if (Rect::isPointInSide(tileCentreP, points[0], rectWidth, rectHeight)) //
                    {                                                                       // is the center rect of the tile.
                        // remember the centre rect for each tile.
                        tileCentreMap[cKeys[0].x][cKeys[0].y] = &hMap[x][y];
                        hMap[x][y].height = typeHeight;
                    }
                    else if (tl0.type == CellTypes::OCEAN || tl0.type == CellTypes::SHORE)
                    {
                        // for ocean , the height should be fixed.
                        hMap[x][y].height = typeHeight;
                    }
                    else // not the centre rect, check all the corner's tile type.
                    {
                        // check if 4 corner's type is same;
                        // check if this rect is inside a certain type.
                        CellType type0 = hMap[x][y].types[0];
                        CellType type1 = hMap[x][y].types[1];
                        CellType type2 = hMap[x][y].types[2];

                        if (type0 == type1 && type1 == type2) // 3 types are same , the entire rect is in same cell or the same types of cell.
                        {
                            hMap[x][y].height = typeHeight;
                        }
                        else
                        { // calculate
                            float h = calculateRectHeightBySamples(points[0], [this, &tiles](CellKey cKey)
                                                                   {
                                                                       int tx = std::clamp<int>(cKey.x, 0, tWidth - 1);
                                                                       int ty = std::clamp<int>(cKey.y, 0, tHeight - 1);

                                                                       CellData &ttl = tiles[tx][ty];
                                                                       return defineTileHeight(ttl); //
                                                                   });
                            hMap[x][y].height = h;
                        }
                        //
                    }

                    //
                }
            }
            // calculate the height of non-centre but the inner circle is inside the same cell.
            /*
             */
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    if (!hMap[x][y].isHeightResolved())
                    {

                        float distanceToTileCentre = hMap[x][y].originInTile.length();
                        // rad of the tile is 1.0;
                        if (distanceToTileCentre + rectRad < 1.0)
                        {
                            int tx = hMap[x][y].cKey.x;
                            int ty = hMap[x][y].cKey.y;
                            CellsVertex *centreRect = tileCentreMap[tx][ty];
                            assert(centreRect && "centreRest is missing?");

                            hMap[x][y].height = centreRect->height; // use the same height as the centre rect.
                        }
                    }
                }
            }
            // calculate the height of non-centre by neiber's height.
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    CellsVertex &vertex = hMap[x][y];

                    if (!vertex.isHeightResolved())
                    { // is the centre rect
                        // find all neiber tiles , and include the current till as well.
                        // 6+1 = 7 as the max size;
                        CellKey cKey = hMap[x][y].cKey;
                        float sumHeight = 0.0f;
                        int validNeibers = 0;
                        float sumWeight = 0;
                        auto visit = [&x, &y, &vertex, &validNeibers, &sumHeight, &tileCentreMap, &sumWeight](int idx, int nTX, int nTY)
                        {
                            CellsVertex *nTileCentre = tileCentreMap[nTX][nTY];
                            if (nTileCentre)
                            { // if centre rect exist for this current neiber.
                                float distance = vertex.distance(nTileCentre->cKey);
                                float weight = 1.0f / (distance + 1e-6);
                                weight = 1.0f;
                                sumHeight += nTileCentre->height * weight;
                                validNeibers++;
                                sumWeight += weight;
                                if (DEBUG_COUT)
                                {
                                    std::cout << fmt::format("Vertex[{},{}],Neiber[{:>2}].cKey({},{}) .height:{}, .dis:{}, sumHeight:{},sumWeight:{}", x, y, idx, nTX, nTY, nTileCentre->height, distance, sumHeight, sumWeight) << std::endl;
                                }
                            }
                        };
                        if (DEBUG_COUT)
                        {
                            std::cout << fmt::format("===================") << std::endl;
                        }

                        int neibers = Cell::forEachNeibers(cKey.x, cKey.y, tWidth, tHeight, visit); // visit neibers.
                        visit(-1, cKey.x, cKey.y);                                                  // visit this cell.
                        assert(validNeibers > 0 && "bug?");
                        float height = sumHeight / sumWeight;
                        if (DEBUG_COUT)
                        {
                            std::cout << fmt::format("::::::::::{}:::::::::", height) << std::endl;
                        }
                        hMap[x][y].height = height;
                    }
                }
            }
        }

        /**
         *
         */
        float defineTileHeight(CellData &tl)
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
                                           std::function<float(CellKey)> getHeight)
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
                    Ogre::Vector2 p(
                        center.x + (u - 0.5f) * rectWidth,
                        center.y + (v - 0.5f) * rectHeight);

                    // HexTile::Key k = Cell::getCellKey(p, 1); //
                    CellKey k = Point2<float>(p.x, p.y).transform(Transform::CentreToCellKey());

                    totalHeight += getHeight(k);
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
