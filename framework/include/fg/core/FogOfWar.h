/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "fg/core/TextureFactory.h"
#include "fg/util/CollectionUtil.h"
#include "fg/util/Box2.h"
#include "Ogre.h"
#include "fg/util.h"
#include "fg/demo/HomeCellKey.h"
namespace fog
{

    struct FogOfWar
    {
        struct Options
        {

            int width;
            int height;

            int tlsWidth;
            int tlsHeight;
            std::string texName;

            INJECT(Options()) : tlsWidth(Config::TILES_RANGE.getWidth()), tlsHeight(Config::TILES_RANGE.getHeight()),
                                width(Config::FOG_OF_WAR_TEX_RANGE.getWidth()), height(Config::FOG_OF_WAR_TEX_RANGE.getHeight()),
                                texName("FogOfWarTex")
            {
            }
        };

        int width;
        int height;

        int tlsWidth;
        int tlsHeight;

        unsigned char *data;   // background.
        unsigned char *buffer; // buffer capacity is defined by bufferBox, in edige of the world the buffer is not fully used.
        std::string texName;
        TexturePtr texture;
        Box2<int> bufferBox{0}; // moving around. but keep the width * height, even if the box is moving out the world.
        //
        HomeCellKey * homeCellKey;
        Event::Bus *eventBus;

    public:
        INJECT(FogOfWar(Options opts, Event::Bus *eventBus,HomeCellKey * homeCellKey)) : tlsWidth(opts.tlsWidth), tlsHeight(opts.tlsHeight),
                                                               eventBus(eventBus),
                                                            homeCellKey(homeCellKey),
                                                               width(opts.width), height(opts.height), // texture width.
                                                               texName(opts.texName),
                                                               buffer(nullptr) //
        {
            this->data = new unsigned char[width * height * 4]; // rgba;
            // to speed up calculation.
           
            
            // Box2<float> box = HexTile::Key(0, 0).getOuterBoxInUV(tlsWidth, tlsHeight); // cover the entire tile.
            // // scale from centre of the box, p1 is (0,0),p2 is very small value some thing like: 1/cells*rad.
            // box.expand(3.0); // expand to 3 cell width.
            
            // // find the position of p2 in texture, and the width now is the numbers of pixels.
            // box.scale(width, height);
            
            // int bW = box.getWidth();
            // int bH = box.getHeight();
            // // now the box is ready for moving, do not change the width and height.
            // this->bufferBox = Box2<int>(bW, bH); // fixed width height.
            // // prepare the buffer for texture update.
            
            this->bufferBox = buildBufferBox(homeCellKey->cKey);

            Box2<int> homeBox = bufferBox;
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    int idx = (y * width + x) * 4;
                    if (Config::FOG_OF_WAR_EREASE_RANGE.isIn(x, y) || homeBox.isIn(x, y))
                    {
                        data[idx] = 255; // R turn light on.
                    }
                    else
                    {
                        data[idx] = 0; // R
                    }
                    data[idx + 1] = 0; // G
                    data[idx + 2] = 0; // B
                    data[idx + 3] = 0; // A
                } // for
            } // for            
            this->buffer = new unsigned char[bufferBox.getWidth() * bufferBox.getHeight() * 4]; // fixed capacity.                            

            this->texture = TextureFactory::createTexture(texName, width, height, this->data);

            

            eventBus-> //
                subscribe<MovableEventType, State *>([this](MovableEventType evtType, State *state)
                                                     {
                                                         if (evtType == MovableEventType::StateMoved)
                                                         {
                                                             //
                                                             this->onMoving(state);
                                                         }
                                                         else if (evtType == MovableEventType::StateStopMoving)
                                                         {
                                                             this->onMoving(state);
                                                         }

                                                         return true; //
                                                     });

        }
        
        ~FogOfWar()
        {
            delete[] this->data;            
            if(this->buffer){
                delete[] this->buffer;
            }
            
        }
        std::string getTexName()
        {
            return texName;
        }
        
        public:
        
        Box2<int> buildBufferBox(CellKey cKey)
        {
            Box2<float> box = cKey.getOuterBoxInUV(tlsWidth, tlsHeight); // cover the entire tile.
            // scale from centre of the box, p1 is (0,0),p2 is very small value some thing like: 1/cells*rad.
            box.expand(3.0); // expand to 3 cell width.

            // find the position of p2 in texture, and the width now is the numbers of pixels.
            box.scale(width, height);
            return box.cast<int>();
        }

        void set(CellKey cKey, bool visible)
        {

            if (Config::DEBUG_FOG_OF_WAR && Config::DEBUG_COUT)
            {
                // std::cout << fmt::format("fogOfWar[{:>2}{:>2}][{:>2},{:>2};{:>2},{:>2}]", cKey.x, cKey.y, x1, y1, x2, y2) << std::endl;
            }
            // TODO make a new space to transform from ckey to uv.
            // Point2<float> centreP = Cell::getOriginUV(cKey, tlsWidth, tlsHeight);
            //
            Point2<float> centreP = cKey.getCentre().transform(Transform::D2CellWorldUV(tlsWidth, tlsHeight));

            centreP.scale(width, height); // find the position in texture space.
            // Point2<float> centreP = cKey.getCentre().scale(width,height);

            Box2<int> box2 = this->bufferBox;

            // move to the target position
            box2.moveCentreTo(Point2<int>(centreP.x, centreP.y)); //

            // tailering according to the texture.
            Box2<int> entireTexBox(width, height); //

            Box2<int> dBox1 = Box2<int>::intersection(entireTexBox, this->bufferBox);
            Box2<int> dBox2 = Box2<int>::intersection(entireTexBox, box2);

            // update texture

            update(bufferBox.getCentre(), dBox1, false); // recover the box hidden.
            update(box2.getCentre(), dBox2, true);       // open the new box

            this->bufferBox = box2;
        }

        void update(Point2<int> centre, Box2<int> dBox, bool visible)
        {
            int dWidth = dBox.getWidth();
            int dHeight = dBox.getHeight();

            int visibales = 0;
            float rad = bufferBox.getWidth() / 2.0; //
            for (int x = 0; x < dWidth; x++)
            {
                for (int y = 0; y < dHeight; y++)
                {
                    // distance to centre.
                    bool v = visible;
                    if (visible)
                    {
                        Point2<int> p{x, y};
                        p = p + dBox.p1;

                        if (centre.distance(p) > rad) // corner is invisible
                        {
                            v = false;
                        }
                    }
                    if (v)
                    {
                        visibales++;
                    }
                    Point2<int> texPos = dBox.p1 + Point2<int>{x, y};
                    int texIdx = texPos.y * width + texPos.x;

                    unsigned char r = 255; // light on
                    if (v)
                    {
                        // remember this light & position.
                        data[texIdx] = 125; // R of texture pix, this position is light ever,thus not hidden enirely.
                    }
                    else
                    {
                        r = data[texIdx]; // restore from background.
                    }

                    setData(dWidth, dHeight, x, y, r);
                }
            }
            if (visible && visibales < 1)
            {
                // std::runtime_error("bug?");
                std::cout << "visible or not?" << std::endl;
            }

            // this is the centre index in texture data.
            // caculate the outer box to cover entire tile.

            TextureFactory::updateTexture(texture, dWidth, dHeight, buffer, dBox); //, Range2<int>(x1, y1, x2, y2));
        }

        void onMoving(State *state)
        {
            // HexTile::Key cis;
            // if (Context<Cell::Center>::get()->findCellByPosition(state->getPosition(), cis))
            // {
            //     this->set(cis, true);
            // }
            CellKey cis = CellKey::from(state->getPosition());
            this->set(cis, true);
        }

    private:
        void setData(int w, int h, int x, int y, unsigned char r)
        {
            int idx = (y * w + x) * 4;
            buffer[idx] = r;
        }

        void updateTexture()
        {
            TextureFactory::updateTexture(texture, width, height, data);
        }
    };
}
