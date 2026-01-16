
#include "fg/e01.h"

namespace fog
{

    int Example01::run(Options::Groups &ogs)
    {
        std::cout << "OGRE Version: "
                  << OGRE_VERSION_MAJOR << "."
                  << OGRE_VERSION_MINOR << "."
                  << OGRE_VERSION_PATCH << std::endl;

        std::cout << "Weighted Hexagonal Grid Navigation System\n";
        std::cout << "=========================================\n\n";

        {
            std::any av = ogs.groups["building#args"].getOption("scale")->getValue();
            float fv = std::any_cast<float>(av);

            Injector injector;
            injector.bindPtr<Injector>(&injector);
            injector.bindFunc<Options::Groups>([&ogs]()
                                               { return &ogs; });

            injector.bindImpl<App, Example01>();
            injector.bindImpl<ImGuiAppContext::Args>();
            injector.bindArgOfConstructor<std::string, ImGuiAppContext::Args>([]() -> std::string *
                                                                              { return new std::string("e01"); });
            injector.bindImpl<ImGuiAppContext, Example01::TheImGuiAppContext>();

            injector.bindImpl<CoreMod, SimpleCore>();                
            injector.bindImpl<ImGuiAppImpl>();
            CoreMod * core = injector.get<CoreMod>();
            Example01::setup(injector);
            injector.get<Example01>();
            core->startRendering();

            App *app = injector.get<App>();
            // Ogre::Root *root = injector.getPtr<CoreMod>()->getRoot();
            // root->startRendering(); //
            // injector.getPtr<CoreMod>()->startRendering();
        }
        return 0;
    }
};
