/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/util.h"
#include "Common.h"
#include "fg/ogre.h"
namespace fog
{

    struct ShaderManager
    {
        INJECT(ShaderManager())
        {
        }

        void setCameraPos(Vector3 pos)
        {
            Ogre::GpuProgramManager &gpuMgr = Ogre::GpuProgramManager::getSingleton();
            Ogre::GpuSharedParametersPtr sParams = gpuMgr.getSharedParameters("FragSharedParams");
            sParams->setNamedConstant<3, float>("cameraPos", pos);
        }
    };
}; //
