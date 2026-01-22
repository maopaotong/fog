/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <Ogre.h>
#include <OgreColourValue.h>
#include <Bites/OgreApplicationContext.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreFrameListener.h>
#include <OgreRTShaderSystem.h>
#include <OgreTechnique.h>
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgreInput.h>
#include <OgreCustomCompositionPass.h>
#include <unordered_map>

namespace fog
{
    using ColourValue = Ogre::ColourValue;
    using AnimationStateIterator = Ogre::AnimationStateIterator;
    using AnimationState = Ogre::AnimationState;
    using Quaternion = Ogre::Quaternion;
    using GpuSharedParametersPtr = Ogre::GpuSharedParametersPtr;
    using Node = Ogre::Node;
    using Ray = Ogre::Ray;
    using SceneNode = Ogre::SceneNode;
};