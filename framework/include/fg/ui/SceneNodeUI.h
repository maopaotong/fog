/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include "fg/ogre.h"

#include "fg/util.h"
#include "fg/demo.h"

#include "imgui.h"
#include <fmt/format.h>
#include <random>
#include <OgreRenderWindow.h>
#include "ActiveTrayUI.h"

namespace fog{
class SceneNodeUI
{
    template <typename... Args>
    using NodeVistFunc = bool (*)(int depth, Ogre::Node *cNode, Args... args);

    template <typename... Args>
    using SceneNodeVistFunc = bool (*)(int depth, Ogre::SceneNode *cNode, Args... args);

    CoreMod *core;
    bool breakRenderRequested = false;
    ActiveTrayUI *activeTrayUI = nullptr;

public:
    SceneNodeUI(CoreMod *core)
    {
        this->core = core;
        // active tray
    }

    void Open()
    {

        ImGui::Begin("SceneNode Explorer");

        Ogre::SceneNode *sNode = core->getRootSceneNode();
        int id = 0;
        SceneNodeVistFunc<int &> func = (SceneNodeVistFunc<int &>)[](int depth, Ogre::SceneNode *cNode, int &id) -> bool
        {
            float indent = (depth + 1) * 10.0f;
            ImGui::Indent(indent);
            ImGui::Text(fmt::format("SceneNode: {}", cNode->getName()).c_str());
            ImGui::SameLine();
            ImGuiUtil::Text(cNode->getPosition());
            Ogre::Any expand = cNode->getUserObjectBindings().getUserAny(".expanding");            
            bool isExpand = expand.isEmpty()?false: expand.get<bool>();
            
            ImGui::PushID(id++);
            if(cNode->getChildren().size()>0){
                ImGui::SameLine();
                if(ImGui::Checkbox("Exp:", &isExpand)){
                    cNode->getUserObjectBindings().setUserAny(".expanding", Ogre::Any(isExpand));
                }
            }

            ImGui::PopID();

            ImGui::Unindent(indent); 
            return isExpand; };
        SceneNodeUI::forEachSceneNode<int &>(0, sNode, func, id);

        ImGui::End();
    }
    template <typename... Args>
    static void forEachSceneNode(int depth, Ogre::SceneNode *cNode, SceneNodeVistFunc<Args...> func, Args... args)
    {

        forEachNode<SceneNodeVistFunc<Args...>, Args...>( //
            depth, cNode,                                 //
            (NodeVistFunc<SceneNodeVistFunc<Args...>, Args...>)[](int depth, Node *cNode, SceneNodeVistFunc<Args...> func, Args... args) -> bool
            {
                Ogre::SceneNode *sceCNode = dynamic_cast<SceneNode *>(cNode);
                if (sceCNode)
                {
                    return func(depth, sceCNode, args...);
                }
                return true;
            },
            func, args...);
    }

    template <typename... Args>
    static void forEachNode(int depth, Ogre::Node *cNode, bool (*func)(int, Ogre::Node *, Args...), Args... args)
    {
        bool goOn = func(depth, cNode, args...);
        if (!goOn)
        {
            return;
        }
        auto &children = cNode->getChildren();
        for (auto &child : children)
        {
            forEachNode<Args...>(depth + 1, child, func, args...);
        }
    }
};
};//end of namespace
