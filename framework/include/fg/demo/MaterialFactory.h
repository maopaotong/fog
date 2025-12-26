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
    //using namespace Ogre;
    //using namespace OgreBites;

    class MaterialFactory
    {
    private:
        
        static Ogre::MaterialPtr createVertexColourMaterialForActor(CoreMod *core)
        {
            //using namespace Ogre;

            Ogre::MaterialPtr mat = core->createMaterial(MaterialNames::materialNameForActor, "General");

            //
            mat->setReceiveShadows(false);

            Ogre::Technique *tech = mat->getTechnique(0);

            Ogre::Pass *pass = tech->getPass(0);
            pass->setLightingEnabled(true);
            pass->setVertexColourTracking(Ogre::TrackVertexColourEnum::TVC_DIFFUSE); // 
            pass->setVertexColourTracking(Ogre::TrackVertexColourEnum::TVC_AMBIENT); // 
            pass->setVertexColourTracking(Ogre::TrackVertexColourEnum::TVC_EMISSIVE);// 
            pass->setVertexColourTracking(Ogre::TrackVertexColourEnum::TVC_SPECULAR); // 
            return mat;
        }
        static Ogre::MaterialPtr createVertexColourMaterialBuilding(CoreMod *core)
        {
            //using namespace Ogre;

            Ogre::MaterialPtr mat = core->createMaterial(MaterialNames::materialNameBuilding, "General");

            mat->setReceiveShadows(true);

            Ogre::Technique *tech = mat->getTechnique(0);

            Ogre::Pass *pass = tech->getPass(0);
            pass->setLightingEnabled(true);
            pass->setVertexColourTracking(Ogre::TrackVertexColourEnum::TVC_NONE //
                                                                          //| TrackVertexColourEnum::TVC_AMBIENT  //
                                                                          //| TrackVertexColourEnum::TVC_EMISSIVE //
            );
            // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_EMISSIVE);//自发光
            // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_SPECULAR);//镜面反射
            // TextureUnitState * texState = pass->createTextureUnitState("Ground23_spec.png");
            // TextureUnitState *sandTex = pass->createTextureUnitState("tusk.jpg");
            // sandTex->setTextureAddressingMode(TextureUnitState::TAM_WRAP);

            Ogre::TextureUnitState *grassTex = pass->createTextureUnitState("11_13.jpg");
            grassTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
            // grassTex->setColourOperationEx(LayerBlendOperationEx::LBX_BLEND_CURRENT_ALPHA); //

            // TextureUnitState * blendTex = pass->createTextureUnitState("grass_1024.png");
            // blendTex->setTextureAddressingMode(TextureUnitState::TAM_WRAP);

            grassTex->setColourOperationEx(
                Ogre::LayerBlendOperationEx::LBX_BLEND_TEXTURE_ALPHA,
                Ogre::LayerBlendSource::LBS_TEXTURE, //
                Ogre::LayerBlendSource::LBS_CURRENT  //
            );

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
            //createVertexColourMaterialForActor(core);    // for actor
            //createVertexColourMaterialForSelected(core); // for selected
            //createVertexColourMaterialBuilding(core);    // for building
        }
    };

}; // end of namespace
