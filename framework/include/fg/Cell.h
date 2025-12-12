/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <Ogre.h>
#include <OgreVector.h>
#include "fg/util/CostMap.h"
#include <OgreManualObject.h>
#include <OgreColourValue.h>
#include "fg/Terrains.h"
#include <cmath>
#include <algorithm>
#include "fg/util/Box2.h"
#include "fg/Config.h"
#include "fg/util/HexTile.h"

using namespace Ogre;

namespace fog
{

    class Cell
    {

    public:
        template <typename F>
        static int forEachNeibers(int x, int y, int width, int height, F &&visit)
        {
            HexTile::Key neibers[6];
            Cell::getNeibers(x, y, neibers);
            //
            int count = 0;
            for (int i = 0; i < 6; i++)
            {
                int nX = neibers[i].x;
                int nY = neibers[i].y;

                if (nX >= 0 && nX < width && nY >= 0 && nY < height)
                {
                    visit(i, nX, nY);
                    count++;
                }
            }

            return count;
        }

        /**
         *
         *    * * *     [0,3]     [1,3]    [2,3]
         *   * * *    [0,2]   [1,2]    [2,2]
         *    * * *     [0,1]     [1,1]    [2,1]
         *   * * *    [0,0]   [1,0]    [2,0]
         *
         */
        static void getNeibers(int x, int y, HexTile::Key *neibers)
        {

            // e.g. :[1,2]
            if (y % 2 == 0)
            {

                neibers[0].x = x; //[1,1]
                neibers[0].y = y - 1;

                neibers[1].x = x + 1; //[2,2]
                neibers[1].y = y;

                neibers[2].x = x; //[1,3]
                neibers[2].y = y + 1;

                neibers[3].x = x - 1; //[0,3]
                neibers[3].y = y + 1;

                neibers[4].x = x - 1; //[0,2]
                neibers[4].y = y;

                neibers[5].x = x - 1; //[0,1]
                neibers[5].y = y - 1;
            }
            else
            { // e.g. [1,1]

                neibers[0].x = x + 1; //[2,0]
                neibers[0].y = y - 1;

                neibers[1].x = x + 1; //[2,1]
                neibers[1].y = y;

                neibers[2].x = x + 1; //[2,2]
                neibers[2].y = y + 1;

                neibers[3].x = x; //[1,2]
                neibers[3].y = y + 1;

                neibers[4].x = x - 1; //[0,1]
                neibers[4].y = y;

                neibers[5].x = x; //[1,0]
                neibers[5].y = y - 1;
            }
        }


        // static Point2<float> getPointInUV(Point2<float> pIn2D, int width, int height)
        // {
        //     //return Point2<float>(pIn2D.x / (2.0 * static_cast<float>(width)), pIn2D.y / (std::sqrt(3.0) * static_cast<float>(height)));
        //     return pIn2D.transform(Transform::D2CellWorldUV(width,height));
        // }

        // static Point2<float> getPointInUV(Ogre::Vector2 pIn2D, int width, int height)
        // {

        //     return Point2<float>(pIn2D.x / (2.0 * static_cast<float>(width)), pIn2D.y / (std::sqrt(3.0) * static_cast<float>(height)));
        // }

        // static Point2<float> getOriginUV(HexTile::Key cKey, int width, int height)
        // {
        //     Vector2 pIn2D = getOrigin2D(cKey);
        //     //
        //     // total width  in 2d = 2.0 * width in num;
        //     // total height in 2d = sqrt3 * width in num;
        //     // return cKey.cast<float>().transform(Config::HexTile::Key_2_UV);

        //     return getPointInUV(pIn2D, width, height);
        // }

        // static Box2<float> getBoxInUV(HexTile::Key cKey, int width, int height)
        // {
        //     Box2<float> r = cKey.getBoxIn2D();
        //     //r.scale(1.0 / (2.0 * static_cast<float>(width)), 1.0 / (std::sqrt(3.0) * static_cast<float>(height)));
        //     //return r;
        //     return r.transform(Transform::D2CellWorldUV(width, height));
        // }
        // static Box2<float> getOuterBoxInUV(HexTile::Key cKey, int width, int height)
        // {
        //     Box2<float> r = cKey.getOuterBoxIn2D();
        //     //return r.transform(Transform::Scale2<float>(1.0 / (2.0 * static_cast<float>(width)), 1.0 / (std::sqrt(3.0) * static_cast<float>(height))));
        //     return r.transform(Transform::D2CellWorldUV(width,height));

        //     // r.scale(1.0 / (2.0 * static_cast<float>(width)), 1.0 / (std::sqrt(3.0) * static_cast<float>(height)));
        // }

        // static Box2<float> getBoxIn2D(HexTile::Key cKey)
        // {
        //     // Vector2 centre = getOrigin2D(cKey);
        //     Point2<float> centre = cKey.cast<float>().transform(Transform::CellCentreByKey());
        //     float left = centre.x - 1.0; // rad
        //     float bottom = centre.y - std::sqrt(3.0) / 2.0;
        //     return Box2<float>(left, bottom, left + 2.0, bottom + std::sqrt(3.0));
        // }

        // static Box2<float> getOuterBoxIn2D(HexTile::Key cKey)
        // {
        //     // Vector2 centre = getOrigin2D(cKey);

        //     Point2<float> centre = cKey.cast<float>().transform(Transform::CellCentreByKey());

        //     float left = centre.x - 1.0;                    // rad
        //     float bottom = centre.y - 2.0 / std::sqrt(3.0); // R
        //     return Box2<float>(left, bottom, left + 2.0, bottom + 4.0 / std::sqrt(3.0));
        // }

        // static Ogre::Vector2 getOrigin2D(HexTile::Key cKey)
        // {
        //     return getOrigin2D(cKey.x, cKey.y);
        // }

        // static Ogre::Vector2 getOrigin2D(int x, int y)
        // {

        //     // float centerX = x * 2 * rad + (y % 2 == 0 ? 0 : rad);
        //     // float centerY = y * rad * std::sqrt(3.0f);
        //     // return Ogre::Vector2(centerX, centerY);
        //     Point2<float> p = HexTile::Key(x, y).cast<float>().transform(Transform::CellCentreByKey()).scale(1);
        //     return p;
        // }

        static HexTile::Key getCellKey(Ogre::Vector2 p)
        {

            return (Point2<float>(p.x, p.y)).transform(Transform::CentreToCellKey());
        }

        static HexTile::Key getCellKey_DEL(Ogre::Vector2 origin2D, float rad)
        {

            // Step 1: 将 pixel 坐标转换为 axial 坐标 (q, r) —— 浮点
            // 对于 flat-topped hex，使用标准公式：
            //   q = (sqrt(3)/3 * x - 1/3 * y) / R
            //   r = (2/3 * y) / R
            // 其中 R 是外接圆半径（中心到顶点）
            // 已知：rad = apothem = R * cos(30°) = R * sqrt(3)/2
            // => R = (2 / sqrt(3)) * rad

            const float sqrt3 = std::sqrt(3.0f);
            const float R = (2.0f / sqrt3) * rad; // 外接圆半径

            float q = ((sqrt3 / 3.0f) * origin2D.x - (1.0f / 3.0f) * origin2D.y) / R;
            float r = ((2.0f / 3.0f) * origin2D.y) / R;

            // Step 2: axial -> cube
            float x = q;
            float z = r;
            float y = -x - z;

            // Step 3: cube rounding to nearest hex
            int rx = std::lround(x);
            int ry = std::lround(y);
            int rz = std::lround(z);

            float dx = std::abs(rx - x);
            float dy = std::abs(ry - y);
            float dz = std::abs(rz - z);

            if (dx > dy && dx > dz)
            {
                rx = -ry - rz;
            }
            else if (dy > dz)
            {
                ry = -rx - rz;
            }
            else
            {
                rz = -rx - ry;
            }

            // Step 4: cube -> odd-r offset coordinates
            // 在 odd-r 中：row = z, col = x + (row - (row & 1)) / 2
            int row = rz;
            int col = rx + (row - (row & 1)) / 2;

            return HexTile::Key(col, row);
        }


    public:
        struct Instance
        {
            HexTile::Key cKey;
            Instance()
            {
            }
            Instance(int x, int y) : Instance(HexTile::Key(x, y))
            {
            }
            Instance(HexTile::Key cKey) : cKey(cKey)
            {
            }

            // // center of the cell.
            // Vector2 getOrigin2D()
            // {
            //     return Cell::getOrigin2D(cKey.x, cKey.y);
            // }

            // Vector3 getOrigin3D(float scale)
            // {
            //     Vector2 origin2D = getOrigin2D();
            //     return node->to3D(origin2D, scale);
            // }
        };

        class Center
        {
            // Node2D *root;
            CostMap *costMap;

        public:
            Center()
            {
                costMap = Context<CostMap>::get();
            }

            // void translateToCenter()
            // {
            //     Vector2 pos2D = this->getCenterIn2D();
            //     Vector3 pos3D = root->to3D(pos2D, 1.0);
            //     root->plane->setOrigin(Vector3(-pos3D.x, 0.0f, -pos3D.z));
            // }

            HexTile::Key getCell(HexTile::Key cKey)
            {
                return cKey;
            }

            HexTile::Key getCell(int x, int y)
            {
                return HexTile::Key(x, y);
            }

            HexTile::Key getAnyCell()
            {
                return HexTile::Key(0, 0);
            }
            // Node2D *getRoot2D()
            // {
            //     return root;
            // }
            
            // std::vector<HexTile::Key> getInstanceListByNomPoints(std::vector<Vector2> pointIn2DNom)
            // {
            //     std::vector<HexTile::Key> ret;
            //     ret.reserve(pointIn2DNom.size());
            //     for (auto pNom : pointIn2DNom)
            //     {
            //         HexTile::Key cell = getCellByNom(pNom);
            //         ret.push_back(cell);
            //     }
            //     return ret;
            // }

            // Vector2 getCenterIn2D()
            // {
            //     float cX = this->costMap->getWidth();
            //     float cY = this->costMap->getHeight();
            //     return Vector2(cX, cY) * root->scale;
            // }

            // HexTile::Key getCellByNom(Vector2 nom)
            // {
            //     return HexTile::Key(static_cast<int>(nom.x), static_cast<int>(nom.y));
            // }

            // bool findCellByPosition(Point2<float> position, HexTile::Key &cell)
            // {
            //     cell = HexTile::Key::from(position);

            //     return true;
            //     // return forCellByPoint(position, [&cell](HexTile::Key &cell2)
            //     //                       { cell = cell2; });


            // }

            // HexTile::Key getCellKeyByPoint(Vector2 pointIn2D)
            // {
            //     return Cell::getCellKey(pointIn2D, root->scale);
            // }

            // template <typename F>
            // bool forCellByPoint(Point2<float> pointIn2D, F &&visit)
            // {

            //     // Vector2 pointIn2D = pointInCell2D * this->node->scale + origin + this->node->position;
            //     // Vector3 positionIn3D = node->plane->to3D(pointIn2D);
            //     // Vector2 pointIn2D = root->to2D(positionIn3D);
            //     // Vector2 pointIn2D = Point2<float>::from(pointIn2D, Transform::D3_NORMAL_D2(Config::D2_NORMAL_D3));

            //     // HexTile::Key cKey = Cell::getCellKey(pointIn2D, Config::CELL_SCALE);
            //     HexTile::Key cKey = Cell::getCellKey(pointIn2D, 1);
            //     int x = cKey.x;
            //     int y = cKey.y;
            //     if (x < 0 || x >= costMap->getWidth() || y < 0 || y >= costMap->getHeight())
            //     {
            //         return false;
            //     }

            //     visit(HexTile::Key(x, y));
            //     return true;
            // }

            template <typename F>
            void forEachCell(F &&visit)
            {
                for (int x = 0; x < costMap->getWidth(); x++)
                {
                    for (int y = 0; y < costMap->getHeight(); y++)
                    {
                        HexTile::Key cell(x, y);
                        visit(cell);
                    }
                }
            }

            int getWidth()
            {
                return this->costMap->getWidth();
            }
            int getHeight()
            {
                return this->costMap->getHeight();
            }
        };
    };

};
