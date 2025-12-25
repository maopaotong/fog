#pragma once
#include "fg/util.h"
namespace fog
{

    struct FogOfWarTexture
    {
        struct Args
        {

            int width;
            int height;

            int tlsWidth;
            int tlsHeight;
            std::string texName;

            INJECT(Args(Config *cfg, CellsDatas::Args &cdos)) : tlsWidth(cdos.cellsRange.getWidth()), tlsHeight(cdos.cellsRange.getHeight()),
                                           width(cfg->fogOfWarTextRange.getWidth()), height(cfg->fogOfWarTextRange.getHeight()),
                                           texName(cfg->FOG_OF_WAR_TEX_NAME)
            {
            }
        };

        Args opts;
        Ogre::TexturePtr texture;
        INJECT(FogOfWarTexture(Args opts)) : opts(opts)
        {
            this->texture = TextureFactory::createTexture(opts.texName, opts.width, opts.height);
        };
    };
};