#pragma once
#include "Common.h"
#include "fg.h"
#include "ogre.h"
#include "CellInstanceStateManager.h"
namespace fog
{
    struct MouseCellController
    {

        CoreMod *core;
        CellInstanceStateManager *cellInstMgrState;
        Transforms *tfs;
        Event::Bus *eventBus;
        INJECT(MouseCellController(CoreMod *core, CellInstanceStateManager *cellInstMgrState, Transforms *tfs, Event::Bus *eventBus)) : core(core), cellInstMgrState(cellInstMgrState), tfs(tfs), eventBus(eventBus)
        {
        }

        // TODO move this function to MouseStateManager
        CONSUMED onMouseMoved(int x, int y)
        {

            // Viewport *viewport = Context<CoreMod>::get()->getViewport();
            // Camera *camera = Context<CoreMod>::get()->getCamera();
            Box2<float> vp = core->getActualViewportBox();

            float ndcX = x / (float)vp.getWidth();
            float ndcY = y / (float)vp.getHeight();

            Ogre::Ray ray = core->getCameraToViewportRay(ndcX, ndcY);

            Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0); // Y = 0

            auto hitGrd = ray.intersects(ground);

            Vector3 pos2;

            if (!hitGrd.first)
            {
                return false;
            }

            pos2 = ray.getPoint(hitGrd.second);

            // Point2<float> p2 = Point2<float>::from(pos2, Transform::D3_NORMAL_D2(config->D2H2D3));
            Point2<float> p2 = Point2<float>::from(pos2, *tfs->d3d2);

            CellInstanceState *cis = cellInstMgrState->getCellInstanceStateByPosition(p2);
            if (!cis)
            {
                return false;
            }

            //cis->pushColour(ColourValue::White);

            eventBus->emit<MouseCellEventType, CellKey>(MouseCellEventType::MouseEnterCell, cis->getCellKey());

            return false;
        }
    };
};