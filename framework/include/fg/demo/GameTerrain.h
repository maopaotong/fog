/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <OgreSceneManager.h>
#include <OgreSharedPtr.h>
#include <stb_image_write.h>
#include <fg/State.h>
#include <OgreShaderSubRenderState.h>
#include <OgreShaderRenderState.h>
#include <OgreShaderGenerator.h>
#include "fg/Terrains.h"
#include "fg/util/OgreCode.h"
#include "fg/demo/ModifyHeightImg.h"
#include "fg/util/DiamondSquare.h"

namespace fog
{
    using namespace Ogre;

    //
    // SubRenderStateFactory::~SubRenderStateFactory()
    //{
    //    OgreAssert(mSubRenderStateList.empty(), "Sub render states still exists");
    //}
    // To fix the above unexpected exception when closing the app context:
    //- Do not delete the pointer by shared pointer.
    //- We need to detroy it before app closing.

    class GameTerrains : public State, public Terrains
    {
        const String FFP_Transform = "FFP_Transform";
        const float flatHight = 0.0f;
        const float terrainScale = 10.0f; // height

        float minHeight1 = 0.35f * terrainScale;//shore
        float fadeDist1 = 1;
        float minHeight2 = 0.50f * terrainScale;//land
        float fadeDist2 = 1;
        float minHeight3 = 0.91f * terrainScale;//mountains
        float fadeDist3 = 1;

        bool flat = false; // for test.
    private:
        long terrainX = 1;
        long terrainY = 1;
        // int terrainSize = 513;
        int terrainSize = 513;

        float worldSize = 2000.0f;
        Ogre::TerrainGroup *terrainGroup = nullptr;
        Ogre::TerrainGlobalOptions *options = nullptr;

        TerrainMaterialGeneratorA *defaultTMG = nullptr;

    public:
        virtual ~GameTerrains()
        {

            if (terrainGroup)
            {
                delete terrainGroup;
            }
            if (options)
            {
                delete options;
            }
            // Trying to fix the exception when closing the app context.
            // Ogre::RTShader::SubRenderStateFactory *fac = Ogre::RTShader::ShaderGenerator::getSingleton().getSubRenderStateFactory(FFP_Transform);
            // fac->destroyAllInstances();
        }

    public:
        void load(RenderSystem *renderSys, Ogre::SceneManager *sceMgr, Light *light)
        {
            // terrain group
            terrainGroup = new Ogre::TerrainGroup(
                sceMgr,
                Ogre::Terrain::ALIGN_X_Z,
                terrainSize,
                worldSize);
            terrainGroup->setOrigin(Ogre::Vector3::ZERO);

            // Config =======================================
            options = new Ogre::TerrainGlobalOptions();
            options->setMaxPixelError(8);
            options->setCompositeMapDistance(3000);

            defaultTMG = new TerrainMaterialGeneratorA();
            TerrainMaterialGeneratorPtr defaultTMGPtr(defaultTMG, [](TerrainMaterialGenerator *ptr)
                                                      { delete ptr; });
            options->setDefaultMaterialGenerator(defaultTMGPtr);

            TerrainMaterialGeneratorA::SM2Profile *matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile *>(
                defaultTMG->getActiveProfile());

            // Disable the lightmap for OpenGL ES 2.0. The minimum number of samplers allowed is 8(as opposed to 16 on
            // desktop). Otherwise we will run over the limit by just one. The minimum was raised to 16 in GL ES 3.0.
            if (renderSys->getCapabilities()->getNumTextureUnits() < 9)
            {
                matProfile->setLightmapEnabled(false);
            }

            // Disable steep parallax by default
            matProfile->setLayerParallaxOcclusionMappingEnabled(false);

            options->setLightMapDirection(light->getDerivedDirection());
            // options->setCompositeMapAmbient(sceMgr->getAmbientLight());
            options->setCompositeMapDiffuse(light->getDiffuseColour());

            Ogre::Terrain::ImportData &defaultimp = terrainGroup->getDefaultImportSettings();
            defaultimp.inputScale = terrainScale;
            defaultimp.minBatchSize = 33;
            defaultimp.maxBatchSize = 65;

            // Ogre::Image combined;
            // combined.loadTwoImagesAsRGBA("Ground23_col.jpg", "Ground23_spec.png", "General");
            // TextureManager::getSingleton().loadImage("Ground23_diffspec", "General", combined);

            defaultimp.layerList.resize(4);

            defaultimp.layerList[0].worldSize = 200;
            defaultimp.layerList[1].worldSize = 200;
            defaultimp.layerList[2].worldSize = 200;
            defaultimp.layerList[3].worldSize = 200;

            defaultimp.layerList[0].textureNames.push_back("G0_col.png");
            defaultimp.layerList[0].textureNames.push_back("G0_norm.png");
            defaultimp.layerList[1].textureNames.push_back("G1_col.png");
            defaultimp.layerList[1].textureNames.push_back("G1_norm.png");
            defaultimp.layerList[2].textureNames.push_back("G2_col.png");
            defaultimp.layerList[2].textureNames.push_back("G2_norm.png");
            defaultimp.layerList[3].textureNames.push_back("G3_col.png");
            defaultimp.layerList[3].textureNames.push_back("G3_norm.png");

            /*
            defaultimp.layerList[0].textureNames.push_back("G0_col.dds");
            defaultimp.layerList[0].textureNames.push_back("G0_norm.dds");
            defaultimp.layerList[1].textureNames.push_back("G1_col.dds");
            defaultimp.layerList[1].textureNames.push_back("G1_norm.dds");
            defaultimp.layerList[2].textureNames.push_back("G3_col.dds");
            defaultimp.layerList[2].textureNames.push_back("G3_norm.dds");
            defaultimp.layerList[3].textureNames.push_back("G2_col.jpg");
            defaultimp.layerList[3].textureNames.push_back("G2_norm.dds");
            */

            // Define =====================================
            for (long x = 0; x < terrainX; ++x)
            {
                for (long y = 0; y < terrainY; ++y)
                {
                    defineTerrain(x, y, flat);
                }
            }
            // load
            terrainGroup->loadAllTerrains(true);
            // material
            // Ogre::Terrain::ImportData &data =
            terrainGroup->getDefaultImportSettings();

            // blend
            for (const auto &ti : terrainGroup->getTerrainSlots())
            {
                initBlendMaps(ti.second->instance);
            }
            // free
            terrainGroup->freeTemporaryResources();
        }
        void initBlendMaps(Ogre::Terrain *terrain)
        {
            Ogre::TerrainLayerBlendMap *blendMap0 = terrain->getLayerBlendMap(1);
            Ogre::TerrainLayerBlendMap *blendMap1 = terrain->getLayerBlendMap(2);
            Ogre::TerrainLayerBlendMap *blendMap2 = terrain->getLayerBlendMap(3);

            float *pBlend0 = blendMap0->getBlendPointer();
            float *pBlend1 = blendMap1->getBlendPointer();
            float *pBlend2 = blendMap2->getBlendPointer();

            int size = terrain->getLayerBlendMapSize();
            for (Ogre::uint16 y = 0; y < size; ++y)
            {
                for (Ogre::uint16 x = 0; x < size; ++x)
                {
                    float u = static_cast<float>(x) / size;
                    float v = 1 - static_cast<float>(y) / size;
                    float height = terrain->getHeightAtTerrainPosition(u, v);

                    // Layer 0
                    float val = (height - minHeight1) / fadeDist1;
                    val = Ogre::Math::Clamp(val, 0.0f, 1.0f);
                    pBlend0[y * size + x] = val;

                    // Layer 1
                    val = (height - minHeight2) / fadeDist2;
                    val = Ogre::Math::Clamp(val, 0.0f, 1.0f);
                    pBlend1[y * size + x] = val;
                    // Layer 2
                    val = (height - minHeight3) / fadeDist3;
                    val = Ogre::Math::Clamp(val, 0.0f, 1.0f);
                    pBlend2[y * size + x] = val;
                }
            }

            blendMap0->dirty();
            blendMap1->dirty();
            blendMap2->dirty();

            blendMap0->update();
            blendMap1->update();
            blendMap2->update();
        }

        float getHeightWithNormalAtWorldPosition(Vector3 posInWld, Vector3 *norm) override
        {
            long x, y;
            terrainGroup->convertWorldPositionToTerrainSlot(posInWld, &x, &y);
            if (x != 0 || y != 0)
            {
                if (norm)
                {
                    *norm = Vector3(0, 1, 0);
                }
                return 0;
            }

            Terrain *ter = terrainGroup->getTerrain(0, 0);

            int size = ter->getSize();

            Vector3 posInTer;
            ter->getTerrainPosition(posInWld, &posInTer);
            struct HeightAtPoint
            {
                Terrain *ter;
                float operator()(uint32 x, uint32 y)
                {
                    return ter->getHeightAtPoint(x, y);
                }
            } getHeightAtPoint = {ter};
            return OgreCode::getHeightAtTerrainPosition(posInTer.x, posInTer.y, getHeightAtPoint, size, norm, true);
        }

        Vector3 getOrigin() override
        {
            return terrainGroup->getOrigin();
        }

        void defineTerrain(long x, long y, bool flat)
        {
            if (flat)
            {
                terrainGroup->defineTerrain(x, y, flatHight);
                return;
            }
            Image img;
            getTerrainImage(x % 2 != 0, y % 2 != 0, img);
            ModifyHeightImg modHeight(img);
            // modHeight.modifyHeight();
            DiamondSquare::generateImg(img);
            terrainGroup->defineTerrain(x, y, &img);
        }

        void getTerrainImage(bool flipX, bool flipY, Image &img)
        {
            //! [heightmap]
            img.load("heightmap.png", terrainGroup->getResourceGroup());

            if (flipX)
                img.flipAroundY();
            if (flipY)
                img.flipAroundX();
            //! [heightmap]
        }

        void genereatePng(const char *filename, int width, int height)
        {
            // 创建一个宽度x高度大小的灰度图像数据容器
            std::vector<uint8_t> image(width * height);

            // 填充图像数据：这里我们简单地填充渐变颜色作为示例
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    // 使用任意方式计算像素值，这里使用了一个简单的线性渐变
                    image[y * width + x] = static_cast<uint8_t>((x + y) % 256);
                }
            }

            // 使用 stb_image_write 保存为 PNG 文件
            stbi_write_png(filename, width, height, 1, image.data(), width); // 最后一个参数是每行字节数
        }
    };
}; // end of namespace
