/*-----------------------------------------------------------------------------
Copyright (c) 2025  Mao-Pao-Tong Workshop

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------*/
#include "fg/util/Context.h"
#include "fg/util.h"
#include "fg/CoreMod.h"
#include "fg/CellInstanceManager.h"
#include "fg/MovingStateManager.h"
#include "fg/MovableStateManager.h"
#include "fg/PathingStateManager.h"
#include "fg/demo/InputStateController.h"
#include "fg/BuildingStateManager.h"
#include "fg/InventoryStateManager.h"
#include "fg/CameraState.h"
#include "fg/core/TilesTerrains.h"
#include "fg/core/FogOfWar.h"
namespace fog
{
    // If the var here not initialised, app must provide the value.
    //

    template <>
    Var<float>::Bag *Context<Var<float>::Bag>::ctxObj = new Var<float>::Bag();

    template <>
    Var<Vector3>::Bag *Context<Var<Vector3>::Bag>::ctxObj = new Var<Vector3>::Bag();

    template <>
    Property::Bag *Context<Property::Bag>::ctxObj = new Property::Bag();

    template <>
    Terrains *Context<Terrains>::ctxObj = nullptr;

    template <>
    Event::Bus *Context<Event::Bus>::ctxObj = new Event::Bus();

    //template <>
    //Node2D *Context<Node2D>::ctxObj = nullptr;

    template <>
    Cell::Center *Context<Cell::Center>::ctxObj = nullptr;

    template <>
    CoreMod *Context<CoreMod>::ctxObj = nullptr;

    template <>
    CostMap *Context<CostMap>::ctxObj = nullptr;
    //
    template <>
    State *Context<State>::ctxObj = nullptr;

    template <>
    Plane *Context<Plane>::ctxObj = nullptr;

    template <>
    MovingStateManager *Context<MovingStateManager>::ctxObj = new MovingStateManager();

    template <>
    CellInstanceManager *Context<CellInstanceManager>::ctxObj = new CellInstanceManager();

    template <>
    MovableStateManager *Context<MovableStateManager>::ctxObj = new MovableStateManager();

    template <>
    PathingStateManager *Context<PathingStateManager>::ctxObj = new PathingStateManager();

    template <>
    InputStateController *Context<InputStateController>::ctxObj = new InputStateController();

    template <>
    BuildingStateManager *Context<BuildingStateManager>::ctxObj = new BuildingStateManager();

    template <>
    InventoryStateManager *Context<InventoryStateManager>::ctxObj = new InventoryStateManager();

    template <>
    CameraState *Context<CameraState>::ctxObj = new CameraState();
    
    template <>
    tiles::Terrains *Context<tiles::Terrains>::ctxObj = nullptr;

    template <>
    Options *Context<Options>::ctxObj = new Options();

    template <>
    FogOfWar *Context<FogOfWar>::ctxObj = nullptr;

    template<>
    Transform::D2H2D3 * Context<Transform::D2H2D3>::ctxObj = nullptr;
    
    template<>
    Transform::D3_NORMAL_D2 * Context<Transform::D3_NORMAL_D2>::ctxObj = nullptr;
    

};
