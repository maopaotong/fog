
#pragma once

#include "fg/util.h"
#include "ImGuiAppImpl.h"
#include "ImGuiAppContext.h"

namespace fog
{

    struct SimpleApp
    {

        template <typename APPCONTEXT, typename CORE, typename APP>
        static int run(Options::Groups &ogs)
        {
            Injector injector;
            injector.bindPtr<Injector>(&injector);
            injector.bindFunc<Options::Groups>([&ogs]()
                                               { return &ogs; });

            injector.bindImpl<ImGuiAppContext::Args>();
            injector.bindArgOfConstructor<std::string, ImGuiAppContext::Args>([]() -> std::string *
                                                                              { return new std::string("e03"); });
            injector.bindImpl<ImGuiAppContext, APPCONTEXT>();
            injector.bindImpl<CoreMod, CORE>();
            injector.bindImpl<ImGuiAppImpl>();
            CoreMod *core = injector.get<CoreMod>();

            injector.bindFunc<SceneNode>([&injector]() -> SceneNode *
                                         { return injector.get<CoreMod>()->getRootSceneNode()->createChildSceneNode(); });
            injector.bindImpl<APP>();

            APP *app = injector.get<APP>();
            core->startRendering();
            std::cout << "done" << std::endl;

            return 0;
        }
    };
};