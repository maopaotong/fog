#pragma once

#include "fg/util.h"
#include "fg/ogre.h"
#include "fg/demo.h"
#include "delaunator/delaunator.h"

namespace fog::examples::e03
{

    static std::vector<double> toFlatPoints(std::vector<PoissonDisk::Point> points)
    {
        std::vector<double> ps2;
        for (PoissonDisk::Point p : points)
        {
            ps2.push_back(p.x);
            ps2.push_back(p.y);
        }
        return ps2;
    }
    using namespace delaunator;
    struct MeshData
    {
        std::vector<PoissonDisk::Point> points;
        Delaunator delaunator;
        int numBoundaryPoints;
        MeshData(std::vector<PoissonDisk::Point> points, int numBoundaryPoints) : points(points),
                                                                                  numBoundaryPoints(numBoundaryPoints),
                                                                                  delaunator(Delaunator{toFlatPoints(points)})
        {
            
        }
    };

    struct TriangleMesh
    {
        TriangleMesh(MeshData &data)
        {
        }
    };

    struct Example : public Ogre::FrameListener
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
                rgm.addResourceLocation("fog/examples/e03/materials", "FileSystem", "App");
            }
        };

        Ogre::ManualObject *obj;
        CoreMod *core;
        Ogre::SceneNode *sceNode;
        SELF(Example)

        INJECT(Example(CoreMod *core, Ogre::SceneNode *sceNode)) : core(core), sceNode(sceNode)
        {
        }
        INIT(setupAll)()
        {
            MeshData data = setupData();
            setupObj(data);
            setupCompositor();
            core->addFrameListener(this);
        }

        MeshData setupData()
        {

            double spacing = 5.5;
            double mountainSpacing = 35;
            double boundarySpacing = spacing * std::sqrt(2);
            int mountainRetries = 30;
            int retries = 6;
            unsigned int seed = 12345;

            PoissonDisk::Bounds bounds(0, 0, 1000, 1000);
            std::vector<PoissonDisk::Point> interiorBoundaryPoints = generateInteriorBoundaryPoints(bounds, boundarySpacing);
            std::vector<PoissonDisk::Point> exteriorBoundaryPoints = generateExteriorBoundaryPoints(bounds, boundarySpacing);

            PoissonDisk::Generator<std::mt19937> mountainPointsGen(bounds, mountainSpacing, mountainRetries, std::mt19937{seed});
            for (auto p : interiorBoundaryPoints)
            {
                if (!mountainPointsGen.add_point(p, spacing * spacing))
                {
                    throw std::runtime_error("failed to add point.");
                }
            }

            std::vector<PoissonDisk::Point> interiorPoints = mountainPointsGen.fill();
            int numMoutainPoints = interiorPoints.size() - interiorBoundaryPoints.size();

            PoissonDisk::Generator<std::mt19937> pointsGen(bounds, spacing, retries, std::mt19937{seed});
            for (auto p : interiorPoints)
            {
                if (!pointsGen.add_point(p))
                {
                    throw std::runtime_error("failed to add point.");
                }
            }

            interiorPoints = pointsGen.fill();

            std::vector<PoissonDisk::Point> points;
            points.insert(points.end(), std::make_move_iterator(exteriorBoundaryPoints.begin()), std::make_move_iterator(exteriorBoundaryPoints.end()));
            points.insert(points.end(), std::make_move_iterator(interiorPoints.begin()), std::make_move_iterator(interiorPoints.end()));

            return MeshData{points, static_cast<int>(exteriorBoundaryPoints.size())};
        }

        std::vector<PoissonDisk::Point> generateInteriorBoundaryPoints(PoissonDisk::Bounds bounds, double boundarySpacing)
        {
            // https://www.redblobgames.com/x/2314-poisson-with-boundary/
            const double epsilon = 1e-4;
            const double curvature = 1.0;
            int W = std::ceil((bounds.width - 2 * curvature) / boundarySpacing);
            int32_t H = std::ceil((bounds.height - 2 * curvature) / boundarySpacing);
            std::vector<PoissonDisk::Point> points;
            // Top and bottom
            for (int q = 0; q < W; q++)
            {
                double t = static_cast<double>(q) / static_cast<double>(W);
                double dx = (bounds.width - 2 * curvature) * t;
                double dy = epsilon + curvature * 4 * std::pow((t - 0.5), 2);
                points.push_back({bounds.x + curvature + dx, bounds.y + dy});
                points.push_back({bounds.x + bounds.width - curvature - dx, bounds.y + bounds.height - dy});
            }
            // Left and right
            for (int r = 0; r < H; r++)
            {
                double t = static_cast<double>(r) / static_cast<double>(H);
                double dy = (bounds.height - 2 * curvature) * t;
                double dx = epsilon + curvature * 4 * std::pow((t - 0.5), 2);
                points.push_back({bounds.x + dx, bounds.y + bounds.height - curvature - dy});
                points.push_back({bounds.x + bounds.width - dx, bounds.y + curvature + dy});
            }
            return points;
        }

        std::vector<PoissonDisk::Point> generateExteriorBoundaryPoints(PoissonDisk::Bounds bounds, double boundarySpacing)
        {
            // https://www.redblobgames.com/x/2314-poisson-with-boundary/
            const double curvature = 1.0;
            const double diagonal = boundarySpacing / std::sqrt(2);
            std::vector<PoissonDisk::Point> points;

            int W = std::ceil((bounds.width - 2 * curvature) / boundarySpacing);
            int H = std::ceil((bounds.height - 2 * curvature) / boundarySpacing);
            // Top and bottom
            for (int q = 0; q < W; q++)
            {
                double t = static_cast<double>(q) / static_cast<double>(W);
                double dx = (bounds.width - 2 * curvature) * t + boundarySpacing / 2;
                points.push_back({bounds.x + dx, bounds.y - diagonal});
                points.push_back({bounds.x + bounds.width - dx, bounds.y + bounds.height + diagonal});
            }
            // Left and right
            for (int r = 0; r < H; r++)
            {
                int t = static_cast<double>(r) / static_cast<double>(H);
                int dy = (bounds.height - 2 * curvature) * t + boundarySpacing / 2;
                points.push_back({bounds.x - diagonal, bounds.y + bounds.height - dy});
                points.push_back({bounds.x + bounds.width + diagonal, bounds.y + dy});
            }
            // Corners
            points.push_back({bounds.x - diagonal, bounds.y - diagonal});
            points.push_back({bounds.x + bounds.width + diagonal, bounds.y - diagonal});
            points.push_back({bounds.x - diagonal, bounds.y + bounds.height + diagonal});
            points.push_back({bounds.x + bounds.width + diagonal, bounds.y + bounds.height + diagonal});
            return points;
        }

        void setupCompositor()
        {
            Ogre::Viewport *vp = core->getViewport();
            Ogre::CompositorManager::getSingleton().addCompositor(vp, "E03Comp01");
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "E03Comp01", true);
        }

        void setupObj(MeshData &data)
        {

            Ogre::MeshPtr meshPtr = Ogre::MeshManager::getSingleton().createManual("LandMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            Ogre::SubMesh *subMesh = meshPtr->createSubMesh();
            subMesh->useSharedVertices = false;
            Ogre::VertexDeclaration *decl = Ogre::HardwareBufferManager::getSingleton().createVertexDeclaration();
            decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
            decl->addElement(0, 4 * 3, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
            decl->addElement(0, 4 * 5, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 1);

            subMesh->vertexData = new Ogre::VertexData();
            subMesh->vertexData->vertexDeclaration = decl;
            size_t vCount = 3;
            subMesh->vertexData->vertexCount = vCount;

            // vertex data
            size_t vSize = decl->getVertexSize(0);
            Ogre::HardwareVertexBufferSharedPtr vBufPtr = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(vSize, vCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
            float *vData = static_cast<float *>(vBufPtr->lock(Ogre::HardwareBuffer::HBL_DISCARD));
            int idx = 0;

            vData[idx++] = 0; // x
            vData[idx++] = 0; // y
            vData[idx++] = 0; // z
            vData[idx++] = 0; // u
            vData[idx++] = 0; // v
            vData[idx++] = 0; // u
            vData[idx++] = 0; // v

            vData[idx++] = 1; // x
            vData[idx++] = 0; // y
            vData[idx++] = 0; // z
            vData[idx++] = 1; // u
            vData[idx++] = 0; // v
            vData[idx++] = 1; // u
            vData[idx++] = 0; // v

            vData[idx++] = 0;  // x
            vData[idx++] = 0;  // y
            vData[idx++] = -1; // z
            vData[idx++] = 0;  // u
            vData[idx++] = -1; // v
            vData[idx++] = 0;  // u
            vData[idx++] = -2; // v

            vBufPtr->unlock();
            subMesh->vertexData->vertexBufferBinding->setBinding(0, vBufPtr);
            // index data.
            int iCount = 3;
            subMesh->indexData->indexCount = iCount;

            subMesh->indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                Ogre::HardwareIndexBuffer::IT_32BIT, // 或 IT_16BIT
                iCount,
                Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            uint32_t *iData = static_cast<uint32_t *>(subMesh->indexData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));
            idx = 0;
            iData[idx++] = 0;
            iData[idx++] = 1;
            iData[idx++] = 2;

            subMesh->indexData->indexBuffer->unlock();

            subMesh->operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;

            // mesh bounds
            meshPtr->_setBounds(Ogre::AxisAlignedBox(-1000, -1000, -1, 1000, 1000, 1));
            meshPtr->_setBoundingSphereRadius(2000);

            // entity
            Ogre::Entity *entity = core->createEntity("LandMesh");
            entity->setMaterialName("E03Mat01");
            sceNode->attachObject(entity);
            sceNode->setScale(30, 30, 30);

            //
        }

        bool frameRenderingQueued(const Ogre::FrameEvent &evt) override
        {

            // obj->setMaterialName(0, "E02Material");
            // obj->setVisible(false);

            return true;
        }

        static int run(Options::Groups &ogs);
    };
};