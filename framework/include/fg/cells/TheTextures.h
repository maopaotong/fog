#pragma once
#include "fg/util.h"
namespace fog
{

    struct FogOfWarTexture
    {
        struct Options
        {

            int width;
            int height;

            int tlsWidth;
            int tlsHeight;
            std::string texName;

            INJECT(Options(Config *cfg, CellsDatas::Options cdos)) : tlsWidth(cdos.cellsRange.getWidth()), tlsHeight(cdos.cellsRange.getHeight()),
                                           width(cfg->fogOfWarTextRange.getWidth()), height(cfg->fogOfWarTextRange.getHeight()),
                                           texName(cfg->FOG_OF_WAR_TEX_NAME)
            {
            }
        };

        Options opts;
        Ogre::TexturePtr texture;
        INJECT(FogOfWarTexture(Options opts)) : opts(opts)
        {
            this->texture = TextureFactory::createTexture(opts.texName, opts.width, opts.height);
        };
    };
};