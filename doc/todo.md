你希望**完全绕过 Ogre 的 Compositor 框架**，用底层 C++ 渲染逻辑手动实现相同效果。这是一个非常合理的需求（尤其当 Compositor 限制太多时）。

下面我将为你提供 **纯 Manual RTT（Render-to-Texture）方式** 的完整替代方案，**1:1 复现你的 Compositor 行为**。

---

## ✅ 目标分解

你的 Compositor 做了 4 步：
1. **渲染高程图** → `tex_elevation`（mask=0x1）
2. **用高程图渲染 draped 内容** → `tex_output1`（mask=0x2）
3. **可选：调试输出** → `tex_output2`（mask=0x4）
4. **最终合成** → 屏幕显示（使用 `tex_elevation` + `tex_output1`）

我们将用 **手动创建 RTT + 切换 viewport + 渲染场景** 实现。

---

## 🛠 完整 C++ 替代代码

### 第一步：创建三个 RenderTexture

```cpp
// 获取主窗口
Ogre::RenderWindow* window = core->getRenderWindow(); // 假设你有这个
Ogre::SceneManager* sceneMgr = core->getSceneManager();

// 获取分辨率
uint32 w = window->getWidth();
uint32 h = window->getHeight();

// 1. 创建 tex_elevation
Ogre::TexturePtr texElevation = Ogre::TextureManager::getSingleton().createManual(
    "tex_elevation",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    Ogre::TEX_TYPE_2D,
    w, h,
    0,                      // mipmaps
    Ogre::PF_BYTE_RGB,
    Ogre::TU_RENDERTARGET
);
Ogre::RenderTexture* rtElevation = texElevation->getBuffer()->getRenderTarget();

// 2. 创建 tex_output1
Ogre::TexturePtr texOutput1 = Ogre::TextureManager::getSingleton().createManual(
    "tex_output1",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    Ogre::TEX_TYPE_2D,
    w, h,
    0,
    Ogre::PF_BYTE_RGB,
    Ogre::TU_RENDERTARGET
);
Ogre::RenderTexture* rtOutput1 = texOutput1->getBuffer()->getRenderTarget();

// 3. 创建 tex_output2（可选，用于调试）
Ogre::TexturePtr texOutput2 = Ogre::TextureManager::getSingleton().createManual(
    "tex_output2",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    Ogre::TEX_TYPE_2D,
    w, h,
    0,
    Ogre::PF_BYTE_RGB,
    Ogre::TU_RENDERTARGET
);
Ogre::RenderTexture* rtOutput2 = texOutput2->getBuffer()->getRenderTarget();
```

---

### 第二步：为每个 RTT 创建专用 Camera 和 Viewport

> 💡 每个 RTT 需要独立的 camera（通常正交）和 viewport

```cpp
// 创建正交相机（用于高程图）
Ogre::Camera* camElevation = sceneMgr->createCamera("CamElevation");
camElevation->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
camElevation->setOrthoWindow(w, h); // 或你的世界尺寸
camElevation->setPosition(0, 0, 100);
camElevation->lookAt(0, 0, 0);

// 为 rtElevation 添加 viewport
Ogre::Viewport* vpElevation = rtElevation->addViewport(camElevation);
vpElevation->setClearEveryFrame(true);
vpElevation->setBackgroundColour(Ogre::ColourValue::Black);
vpElevation->setVisibilityMask(0x00000001); // ← 关键！

// 同样为 rtOutput1 创建 camera（可复用或新建）
Ogre::Camera* camOutput1 = sceneMgr->createCamera("CamOutput1");
camOutput1->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
camOutput1->setOrthoWindow(w, h);
camOutput1->setPosition(0, 0, 100);
camOutput1->lookAt(0, 0, 0);

Ogre::Viewport* vpOutput1 = rtOutput1->addViewport(camOutput1);
vpOutput1->setClearEveryFrame(true);
vpOutput1->setBackgroundColour(Ogre::ColourValue::Black);
vpOutput1->setVisibilityMask(0x00000002);

// tex_output2 同理（visibility_mask = 0x4）
Ogre::Camera* camOutput2 = sceneMgr->createCamera("CamOutput2");
// ... 设置
Ogre::Viewport* vpOutput2 = rtOutput2->addViewport(camOutput2);
vpOutput2->setVisibilityMask(0x00000004);
```

---

### 第三步：设置场景物体的 visibility flags

确保你的物体正确分组：

```cpp
// 高程地形网格
terrainEntity->setVisibilityFlags(0x00000001);

// draped object（如道路、建筑）
drapeEntity->setVisibilityFlags(0x00000002);

// 调试几何体（可选）
debugEntity->setVisibilityFlags(0x00000004);
```

---

### 第四步：每帧渲染流程（代替 Compositor）

在你的主渲染循环中：

```cpp
void renderFrame()
{
    // 1. 渲染高程图到 tex_elevation
    rtElevation->update(); // 这会调用所有 visibility_mask & 0x1 的物体

    // 2. 渲染 draped 内容到 tex_output1
    //    注意：此时你的 draped 材质可以通过 named parameter 使用 texElevation
    bindElevationTextureToDrapeMaterial(texElevation);
    rtOutput1->update(); // 渲染 mask=0x2 的物体

    // 3. （可选）渲染调试内容到 tex_output2
    rtOutput2->update();

    // 4. 最终合成到屏幕
    //    使用一个全屏 quad，材质 E03MatQuad，绑定 texElevation 和 texOutput1
    renderFullscreenQuad(texElevation, texOutput1);
}
```

---

### 第五步：如何让 draped 物体使用 `tex_elevation`

由于你不能用 `input`，必须**手动绑定纹理到 draped 材质**：

```cpp
void bindElevationTextureToDrapeMaterial(const Ogre::TexturePtr& elevationTex)
{
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton()
        .getByName("YourDrapeMaterial"); // 必须匹配你的 draped 物体材质

    if (!mat.isNull())
    {
        Ogre::Pass* pass = mat->getBestTechnique()->getPass(0);
        if (pass->getNumTextureUnitStates() == 0)
            pass->createTextureUnitState();
        
        pass->getTextureUnitState(0)->setTexture(elevationTex);
    }
}
```

> ✅ 现在 `YourDrapeMaterial` 的 fragment shader 就能通过 `sampler2D` 采样高程图了。

---

### 第六步：全屏合成（代替 final render_quad）

你可以用一个 ManualObject 画全屏 quad：

```cpp
void renderFullscreenQuad(
    const Ogre::TexturePtr& tex0,
    const Ogre::TexturePtr& tex1)
{
    static Ogre::ManualObject* quad = nullptr;
    static bool initialized = false;

    if (!initialized)
    {
        quad = sceneMgr->createManualObject("FullscreenQuad");
        quad->setDynamic(false);
        quad->begin("E03MatQuad", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
        quad->position(-1, -1, 0);
        quad->textureCoord(0, 1);
        quad->position(1, -1, 0);
        quad->textureCoord(1, 1);
        quad->position(-1, 1, 0);
        quad->textureCoord(0, 0);
        quad->position(1, 1, 0);
        quad->textureCoord(1, 0);
        quad->end();
        initialized = true;
    }

    // 绑定两个纹理到 E03MatQuad
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("E03MatQuad");
    mat->getBestTechnique()->getPass(0)->getTextureUnitState(0)->setTexture(tex0);
    mat->getBestTechnique()->getPass(0)->getTextureUnitState(1)->setTexture(tex1);

    // 临时添加到场景根节点（或专用合成节点）
    Ogre::SceneNode* node = sceneMgr->getRootSceneNode();
    node->attachObject(quad);
    
    // 渲染到主窗口（确保 visibility mask 允许）
    // （或者直接用 window->update()，因为 quad 在主场景）

    // 注意：如果你主 camera 的 viewport mask 不包含 quad，需临时调整
    // 更简单：确保 quad 的 visibility flags 包含主 viewport 的 mask

    // 渲染完后 detach（或保留）
    // node->detachObject(quad);
}
```

---

## ✅ 优势 vs Compositor

| 功能 | Compositor | 手动 RTT |
|------|-----------|--------|
| 灵活性 | 低（`input` 限制） | **高**（完全控制） |
| 调试 | 困难（黑盒） | **容易**（每步可见） |
| 性能 | 中等 | 相同（底层一样） |
| 代码量 | 少 | 较多 |

---

## 💡 总结

你完全可以**抛弃 Compositor**，用以下步骤替代：

1. **手动创建 3 个 `RenderTexture`**
2. **为每个 RTT 创建专用正交 camera + viewport + visibility mask**
3. **每帧依次调用 `rt->update()`**
4. **通过 C++ 手动绑定纹理到材质（代替 `input`）**
5. **用 ManualObject 全屏 quad 做最终合成**

> 这样你就能**完全掌控渲染流程**，避免 Compositor 的各种隐性限制。

如果你需要，我可以提供一个完整的 `.cpp` 示例文件。这虽然代码更多，但**稳定、透明、可调试** —— 对复杂效果来说，往往是更好的选择。