/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
// main.cpp - Complete Ogre A* Hex Grid Visualization System
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <functional>

// === Include OgreBites for modern initialization ===
#include "fg/ogre.h"
#include "stb_truetype.h"

namespace fog
{
    // === Custom hash function ===
    //
    // === Hexagonal Map Visualizer class ===
    using namespace Ogre;
    using namespace OgreBites;

    class MaterialFactory
    {
    private:
        static void createTexture(RenderSystem *renderSystem)
        {

            // 加载字体文件（如 Roboto.ttf）
            unsigned char *ttf_buffer = new unsigned char[1 << 20];
            FILE *fontFile = fopen("Roboto.ttf", "rb");
            fread(ttf_buffer, 1, 1 << 20, fontFile);
            fclose(fontFile);

            stbtt_fontinfo font;
            stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));

            const char *text = "Building A";
            int width = 256, height = 64;
            unsigned char *pixels = new unsigned char[width * height];

            // 渲染黑白文字（简化版）
            // stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0f, pixels, width, height, 32, 95, cdata);

            // 转为 RGBA（白色文字 + alpha）
            Ogre::uint8 *rgba = new Ogre::uint8[width * height * 4];
            for (int i = 0; i < width * height; ++i)
            {
                rgba[i * 4 + 0] = 255;       // R
                rgba[i * 4 + 1] = 255;       // G
                rgba[i * 4 + 2] = 255;       // B
                rgba[i * 4 + 3] = pixels[i]; // A
            }

            TextureManager *texMgr = TextureManager::getSingletonPtr();

            // 上传到 GPU 纹理
            TexturePtr tex = TextureManager::getSingleton().createManual(
                "DynamicTextTex",
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                Ogre::TEX_TYPE_2D,
                width, height, 0,
                Ogre::PF_BYTE_RGBA,
                Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

            delete[] rgba;
            delete[] pixels;
            delete[] ttf_buffer;
        }
        // 在你的 HexMapVisualizer 构造函数或初始化函数中调用
        static Ogre::MaterialPtr createVertexColourMaterialForSelected(CoreMod *core)
        {
            using namespace Ogre;

            // 创建材质，名称和资源组
            MaterialPtr mat = core->createMaterial(MaterialNames::materialNameSelected, "General");

            // 禁用阴影接收
            mat->setReceiveShadows(false);
            mat->setDepthWriteEnabled(false);
            mat->setTransparencyCastsShadows(false);

            // 获取默认技术（Ogre 2.x 默认会自动创建一个）
            Technique *tech = mat->getTechnique(0);

            // 配置 Pass
            Pass *pass = tech->getPass(0);
            pass->setVertexColourTracking(TrackVertexColourEnum::TVC_EMISSIVE); // 自发光
            pass->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
            pass->setDepthCheckEnabled(false);
            pass->setLightingEnabled(false);
            pass->setSelfIllumination(1, 1, 0.8);
            pass->setDepthBias(1.0f, 0.0f);
            return mat;
        }
        static Ogre::MaterialPtr createVertexColourMaterial(CoreMod * core)
        {
            using namespace Ogre;

            // 创建材质，名称和资源组
            MaterialPtr mat = core->createMaterial(MaterialNames::materialNameToCreate, "General");

            // 禁用阴影接收
            mat->setReceiveShadows(false);

            // 获取默认技术（Ogre 2.x 默认会自动创建一个）
            Technique *tech = mat->getTechnique(0);

            // 配置 Pass
            Pass *pass = tech->getPass(0);
            pass->setLightingEnabled(true);
            pass->setVertexColourTracking(TrackVertexColourEnum::TVC_DIFFUSE); // 漫反射
            // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_AMBIENT);//环境光
            // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_EMISSIVE);//自发光
            // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_SPECULAR);//镜面反射
            return mat;
        }
        static Ogre::MaterialPtr createVertexColourMaterialBuilding(CoreMod *core)
        {
            using namespace Ogre;

            // 创建材质，名称和资源组
            MaterialPtr mat = core->createMaterial(MaterialNames::materialNameBuilding, "General");

            // 禁用阴影接收
            mat->setReceiveShadows(true);

            // 获取默认技术（Ogre 2.x 默认会自动创建一个）
            Technique *tech = mat->getTechnique(0);

            // 配置 Pass
            Pass *pass = tech->getPass(0);
            pass->setLightingEnabled(true);
            pass->setVertexColourTracking(TrackVertexColourEnum::TVC_NONE // 
                                                                          //| TrackVertexColourEnum::TVC_AMBIENT  // 
                                                                          //| TrackVertexColourEnum::TVC_EMISSIVE // 
            );
            // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_EMISSIVE);//自发光
            // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_SPECULAR);//镜面反射
            // TextureUnitState * texState = pass->createTextureUnitState("Ground23_spec.png");
            //TextureUnitState *sandTex = pass->createTextureUnitState("tusk.jpg");
            //sandTex->setTextureAddressingMode(TextureUnitState::TAM_WRAP);

            TextureUnitState *grassTex = pass->createTextureUnitState("11_13.jpg");
            grassTex->setTextureAddressingMode(TextureUnitState::TAM_WRAP);
            //grassTex->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_CURRENT_ALPHA); // 

            // TextureUnitState * blendTex = pass->createTextureUnitState("grass_1024.png");
            // blendTex->setTextureAddressingMode(TextureUnitState::TAM_WRAP);

            grassTex->setColourOperationEx(
                LayerBlendOperationEx::LBX_BLEND_TEXTURE_ALPHA,
                LayerBlendSource::LBS_TEXTURE, //
                LayerBlendSource::LBS_CURRENT // 
            );

            // texState1->setColourOperationEx();
            // texState->setTextureFiltering(Ogre::TFO_ANISOTROPIC);
            // texState->setColourOperation(Ogre::LayerBlendOperation::LBO_MODULATE);

            return mat;
        }

        static Ogre::MaterialPtr createVertexColourMaterialH0085(MaterialManager *matMgr)
        {
            using namespace Ogre;

            // 创建材质，名称和资源组
            MaterialPtr mat = matMgr->create(MaterialNames::materialNameH0085, "General");

            // 禁用阴影接收
            mat->setReceiveShadows(true);

            // 获取默认技术（Ogre 2.x 默认会自动创建一个）
            Technique *tech = mat->getTechnique(0);

            // 配置 Pass
            Pass *pass = tech->getPass(0);
            pass->setLightingEnabled(true);
            pass->setVertexColourTracking(TrackVertexColourEnum::TVC_NONE // 漫反射
                                                                          //| TrackVertexColourEnum::TVC_AMBIENT  // 环境光
                                                                          //| TrackVertexColourEnum::TVC_EMISSIVE // 自发光
            );
            // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_EMISSIVE);//自发光
            // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_SPECULAR);//镜面反射
            // TextureUnitState * texState = pass->createTextureUnitState("Ground23_spec.png");
            //TextureUnitState *sandTex = pass->createTextureUnitState("tusk.jpg");
            //sandTex->setTextureAddressingMode(TextureUnitState::TAM_WRAP);

            TextureUnitState *grassTex = pass->createTextureUnitState("texture_diffuse.png");
            grassTex->setTextureAddressingMode(TextureUnitState::TAM_WRAP);
            //grassTex->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_CURRENT_ALPHA); // 与当前颜色相乘

            // TextureUnitState * blendTex = pass->createTextureUnitState("grass_1024.png");
            // blendTex->setTextureAddressingMode(TextureUnitState::TAM_WRAP);

            // grassTex->setColourOperationEx(
            //     LayerBlendOperationEx::LBX_BLEND_TEXTURE_ALPHA,
            //     LayerBlendSource::LBS_TEXTURE, // 草地纹理
            //     LayerBlendSource::LBS_CURRENT // 当前颜色（即沙地）
            // );

            // texState1->setColourOperationEx();
            // texState->setTextureFiltering(Ogre::TFO_ANISOTROPIC);
            // texState->setColourOperation(Ogre::LayerBlendOperation::LBO_MODULATE);

            return mat;
        }

        void update()
        {
        }

    public:
        static void createMaterials(CoreMod *core)
        {
            //
            // Create hexagonal grid object

            //
            createVertexColourMaterial(core);
            createVertexColourMaterialForSelected(core); // for selected
            createVertexColourMaterialBuilding(core);    // for building
        }
    };

}; // end of namespace
