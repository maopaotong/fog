/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <string>
#include <tuple>
#include <memory>
#include <optional>
#include <any>
#include <fmt/format.h>
#include <iostream>
#include "fg/util/Point2.h"
#include <Ogre.h>
#include <OgreColourValue.h>

namespace fog
{

    // graphics
    // static constexpr float HEIGHT_SCALE = 100.0f;
    // static constexpr int TILES_WIDTH = 128 + 1;
    // static constexpr int TILE_SUBDIVISION_QUALITY = 2;
    // static constexpr float HEIGHT_OCEAN = 0.49f * 0.9f;
    // static constexpr float HEIGHT_SHORE = 0.50f * 0.9f;
    // static constexpr float HEIGHT_PLAIN = 0.51f * 1.1f;
    // static constexpr float HEIGHT_HILL = 0.52f * 1.2f;
    // static constexpr float HEIGHT_MOUNTAIN = 0.53f * 1.f;
    // static constexpr float HEIGHT_FROZEN = 0.54f * 1.f;

    // static constexpr float CELL_SCALE = 30.0f;
    // static constexpr float WORLD_WIDTH = CELL_SCALE * 2.0 * TILES_WIDTH;
    // static constexpr float WORLD_HEIGHT = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
    static constexpr bool DEBUG_COUT = false;
    static constexpr float TEXTURE_COORD_SCALE = 30.0f;
    static constexpr bool DEBUG_CHECK_IMPOSSIBLE_ERROR = false;

    // contoller
    static constexpr float DEFAULT_CAMERA_HEIGHT_MIN = 100.0f;
    static constexpr float DEFAULT_CAMERA_HEIGHT_MAX = 1000.0f * 5;

    static constexpr float DEFAULT_CAMERA_ROLL_SPEED_MIN = 30;
    static constexpr float DEFAULT_CAMERA_ROLL_SPEED_MAX = 300;

    static constexpr float CAMERA_FAR_DISTANCE_MIN = 2000; //
    static constexpr float CAMERA_FAR_DISTANCE_MAX = 5000; //

    // camera speed when move within x-z plane.
    static constexpr float FOG_CAM_SPEED_MIN = 750;
    static constexpr float FOG_CAM_SPEED_MAX = FOG_CAM_SPEED_MIN * 5;

    using GOON = bool;

    using CONSUMED = bool;

    //using Vector2 = Ogre::Vector2;
    
    using Vector2 = Point2<float>;

    using Vector3 = Ogre::Vector3;

    using ColourValue = Ogre::ColourValue;

    using String = std::string;

    template <typename T>
    using List = std::vector<T>;

    template <typename T>
    using UniquePtr = std::unique_ptr<T>;

    template <typename K, typename T>
    using Map = std::unordered_map<K, T>;

    template <typename T>
    using Set = std::unordered_set<T>;

    template <typename T>
    using Table = std::vector<std::vector<T>>;

    enum class MouseCellEventType
    {
        MouseLeaveCell,
        MouseEnterCell,

    };

    enum class CellEventType
    {
        CellAsTarget,
        CellEntered,
        CellLeft,
    };
    enum class MovableEventType
    {
        StatePicked,
        StateUnpicked,
        StateStartMoving,
        StateStopMoving,
        StateEnterCell,
        StateLeaveCell,
        StateMoved,
    };
    enum class BuildingType
    {
        House,
        Farm,
        Barrack,
        Market,
        Tower,
        H0085,
    };
    enum class BuildingEventType
    {
        StatePicked,
        StateUnpicked,
    };

    enum class InventoryType
    {
        BuildingPermit,
        Population,
        Food,
        Building,
        LandScape,
        WaterSource,
        FarmLand,

    };

    const std::unordered_map<InventoryType, std::string> InventoryTypeToString = {
        {InventoryType::BuildingPermit, "BuildingPermit"},
        {InventoryType::Population, "Population"},
        {InventoryType::Food, "Food"},
        {InventoryType::Building, "Building"},
        {InventoryType::LandScape, "LandScape"},
        {InventoryType::WaterSource, "WaterSource"},

    };

};
