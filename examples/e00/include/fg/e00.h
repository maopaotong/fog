#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
namespace fog
{
    struct Example00 : public App
    {
        struct Mod : public fog::Mod
        {
            INJECT(Mod(CoreMod *core, Ogre::SceneNode *sceNode))
            {
                Ogre::ManualObject *obj = core->createManualObject();
                sceNode->attachObject(obj);
                obj->clear();
                obj->begin(MaterialNames::materialNameForCells, Ogre::RenderOperation::OT_TRIANGLE_LIST);
                int baseIdx = obj->getCurrentVertexCount();
                Vector3 a{0, 0, 0};
                Vector3 b{1, 0, 0};
                Vector3 c{0, 0, -1};

                obj->position(a);
                obj->position(b);
                obj->position(c);

                obj->triangle(baseIdx + 0, baseIdx + 1, baseIdx + 2);

                obj->end();
                sceNode->setScale(30, 30, 30);
            }
        };
        struct Setup
        {
            Mod *operator()(Injector &injector)
            {
                injector.bindFunc<SceneNode>([&injector]() -> SceneNode *
                                             { return injector.get<CoreMod>()->getRootSceneNode()->createChildSceneNode(); });
                injector.bindImpl<Example00::Mod>();
                return injector.get<Example00::Mod>();
            }
        };

        INJECT(Example00(Injector *injector))
        {
            this->add(*injector, SimpleCore::Setup());
            this->add(*injector, Example00::Setup()); //
            injector->get<CoreMod>()->startRendering();
        }
        static int run(Options::Groups &ogs);
    };
};