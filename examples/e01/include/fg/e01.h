#pragma once

#include "fg/demo.h"
#include "fg/util.h"
#include "fg/ui.h"
namespace fog
{

    struct CostMapOptions : public CostMap::Options
    {
        INJECT(CostMapOptions(Config *config, CellsDatas::Args &cdos)) : CostMap::Options(cdos.cellsRange.getWidth(), cdos.cellsRange.getHeight())
        {
        }
    };

    struct Example01 : public App
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
                // ApplicationContextBase::locateResources();
                auto &rgm = Ogre::ResourceGroupManager::getSingleton();

                rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/Main", "FileSystem", "OgreInternal");
                rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/RTShaderLib", "FileSystem", "OgreInternal");
                rgm.addResourceLocation("build/vcpkg_installed/x64-windows/share/ogre/Media/Terrain", "FileSystem", "OgreInternal");

                /*
                rgm.addResourceLocation("fog/examples/e00/materials", "FileSystem", "App");
                */
                rgm.addResourceLocation("doc/material", "FileSystem", "App");
                rgm.addResourceLocation("doc/textures", "FileSystem", "General");
                rgm.addResourceLocation("doc/sinbad", "FileSystem", "App");
            }
        };

        
        static void setup(Injector &injector)
        {
            // context binding
            // injector.bindCtx<CellKey>();
            // method binding
            injector.bindMethod<ImGuiApp, CoreMod>(&CoreMod::getImGuiApp);
            // function binding
            injector.bindFunc<SceneNode>([&injector]() -> SceneNode *
                                         { return injector.get<CoreMod>()->getRootSceneNode()->createChildSceneNode(); });
            // as value
            injector.bindImpl<CostMap::Options, CostMapOptions>();

            injector.bindArgOfConstructor<float, TransformD2D3H>([&injector]()
                                                                 { return &injector.get<Config>()->cellScale; } //
            );
            injector.bindArgOfConstructor<float, TransformD3D2>([&injector]()
                                                                { return new float(1.0f / injector.get<Config>()->cellScale); } //
            );

            injector.bindImpl<Building::Args>();
            injector.bindImpl<Geometry::Args>();

            injector.bindAllImpl<
                FogOfWar::Args,
                FogOfWarTexture::Args,
                TransformD2TD3::Args,
                CellsDatas::Args,
                CellsGridsGenerator::Args,
                CellsGenerator::Args,
                CellsMaterial::Args>();

            // impl as pointer
            injector.bindImpl<MovableStateManager>();
            // ui as pointer
            injector.bindAllImpl<
                QuitUI,
                TopBarUI,
                InventoryUI,
                OptionsUI,
                PropertyRefsUI,
                ActiveTrayUI,
                BuildingTrayUI,
                StatisticTrayUI,
                TasksUI>();

            injector.bindImpl<CellsGenerator, AdvCellsGenerator>();
            injector.bindImpl<CellsGridsGenerator, AdvGridsGenerator>();

            // others as pointer
            injector.bindAllImpl<
                BuildingStateManager,
                CameraStateManager,
                CellsMaterial,
                CellInstanceStateManager,
                CellsCost,
                CellsDatas,
                CellsGrids,
                CellsState,
                CellsTexGenerator,
                Config,
                CostMap,
                EntryController,
                EntryUI,
                EntryUI::Children,
                EventBus,
                FogOfWar,
                FogOfWarTexture,
                Geometry,
                InputStateController,
                InventoryManager,
                MovingStateManager,
                MouseCellController,
                OnFrameUI,
                PathingStateManager,
                ShaderManager,
                HomeCellKey,
                Transforms,
                TransformD2D3H,
                TransformD3D2,
                TransformD2TD3,
                WorldTexGenerator>();
            injector.bindImpl<WorldManager, WorldManager>();

            // binding mod.
            injector.bindImpl<Example01, Example01>();
        };

        bool breakRenderRequested = false;

        CoreMod *core;
        OnFrameUI *onFrameUI;
        FogOfWar *fogOfWar;
        WorldManager *world;
        MovingStateManager *movingStateManager;
        // RenderWindow *window;
        CellInstanceStateManager *cellInstances;
        MovableStateManager *msm;
        ImGuiApp *imGuiApp;
        Config *config;
        int cellsCols;
        int cellsRows;

    public:
        INJECT(Example01(OnFrameUI *onFrameUI, FogOfWar *fog, WorldManager *world,
                      //   RenderWindow *window,
                      CellInstanceStateManager *cellInstances,
                      MovableStateManager *msm,
                      CoreMod *core,
                      ImGuiApp *imGuiApp,
                      Config *config,
                      CellsDatas::Args &cdargs,
                      MovingStateManager *movingStateManager))
            : movingStateManager(movingStateManager),
              config(config),
              cellsCols(cdargs.cellsRange.getWidth()),
              cellsRows(cdargs.cellsRange.getHeight()),
              cellInstances(cellInstances),
              imGuiApp(imGuiApp),
              msm(msm),
              core(core),
              //   window(window),
              world(world), fogOfWar(fog), onFrameUI(onFrameUI)
        {
            imGuiApp->addFrameListener(onFrameUI);
            MaterialFactory::createMaterials(core);
            core->addStepListener(movingStateManager);
            // set shader params
            Ogre::GpuProgramManager &gpuMgr = Ogre::GpuProgramManager::getSingleton();
            Ogre::GpuSharedParametersPtr sParams = gpuMgr.getSharedParameters("FragSharedParams");
            sParams->setNamedConstant<int>("showCellEdge", config->debugShaderShowCellEdge);
            sParams->setNamedConstant<int>("showRegionEdge", config->debugShaderShowRegionEdge);
            sParams->setNamedConstant<int>("debug", config->getShaderDebug());
            sParams->setNamedConstant<int>("leiout", CellLayout);
            sParams->setNamedConstant<int>("cellsCols", cellsCols);
            sParams->setNamedConstant<int>("cellsRows", cellsRows);
        }

        virtual ~Example01()
        {
        }

        static int run(Options::Groups &ogs);
    };
};