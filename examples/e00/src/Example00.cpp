
#include "fg/e00.h"
#include <fmt/format.h>

namespace fog
{
    struct TheImGuiAppContext : public ImGuiAppContext
    {
        INJECT(TheImGuiAppContext(Args opts, ImGuiAppImpl *imGuiApp)) : ImGuiAppContext(opts, imGuiApp)
        {
        }

        SELF(TheImGuiAppContext)
        INIT(init)()
        {
            ImGuiAppContext::init();
        }

        void locateResources() override
        {
            auto &rgm = Ogre::ResourceGroupManager::getSingleton();
            rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/Main", "FileSystem", "OgreInternal");
            rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/RTShaderLib", "FileSystem", "OgreInternal");
            rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/Terrain", "FileSystem", "OgreInternal");
            rgm.addResourceLocation("fog/examples/e00/materials", "FileSystem", "App");           
        }
    };

    int Example00::run(Options::Groups &ogs)
    {
        Injector injector;
        injector.bindPtr<Injector>(&injector);
        injector.bindFunc<Options::Groups>([&ogs]()
                                           { return &ogs; });

        injector.bindImpl<ImGuiAppContext::Args>();
        injector.bindArgOfConstructor<std::string, ImGuiAppContext::Args>([]() -> std::string *
                                                                          { return new std::string("e00"); });
        injector.bindImpl<ImGuiAppContext, TheImGuiAppContext>();
        SimpleCore::setup(injector);
        CoreMod *core = injector.get<CoreMod>();

        injector.bindFunc<SceneNode>([&injector]() -> SceneNode *
                                     { return injector.get<CoreMod>()->getRootSceneNode()->createChildSceneNode(); });
        injector.bindImpl<Example00>();

        Example00 *app = injector.get<Example00>();
        core->startRendering();
        std::cout << "done" << std::endl;

        return 0;
    }
};