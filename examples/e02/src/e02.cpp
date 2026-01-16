
#include "fg/e02.h"
#include <fmt/format.h>

namespace fog::examples::e02
{
    
    int Example02::run(Options::Groups &ogs)
    {
        Injector injector;
        injector.bindPtr<Injector>(&injector);
        injector.bindFunc<Options::Groups>([&ogs]()
                                           { return &ogs; });

        injector.bindImpl<ImGuiAppContext::Args>();
        injector.bindArgOfConstructor<std::string, ImGuiAppContext::Args>([]() -> std::string *
                                                                          { return new std::string("e02"); });
        injector.bindImpl<ImGuiAppContext, TheImGuiAppContext>();
        SimpleCore::setup(injector);
        CoreMod *core = injector.get<CoreMod>();

        injector.bindFunc<SceneNode>([&injector]() -> SceneNode *
                                     { return injector.get<CoreMod>()->getRootSceneNode()->createChildSceneNode(); });
        injector.bindImpl<Example02>();

        Example02 *app = injector.get<Example02>();
        core->startRendering();
        std::cout << "done" << std::endl;

        return 0;
    }
};