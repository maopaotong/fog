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
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string>
#include <tuple>
#include <Ogre.h>
#include <OgreColourValue.h>
#include <OgreVector.h>

namespace fog
{
        
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
    static constexpr float FOG_CAM_SPEED_MAX = FOG_CAM_SPEED_MIN * 10;

    using GOON = bool;

    using CONSUMED = bool;

    //using Vector2 = Ogre::Vector2;
    
    //using Vector2 = Point2<float>;

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
        Center,
        Farm,
        Market
    };
    enum class BuildingEventType
    {
        StatePicked,
        StateUnpicked,
    };

    enum class InventoryType
    {
        BuildingPermit,
        ConstructionInProgress,
        Population,
        Food,
        Water,
        Gold,
        Soldier,
        Merchant,
        Taxpayer,
        Army,
        Building,
        WaterSource,
        WaterSourceDay,
        FarmLand,
        Day,
        Worker,
        WorkerDay,
        Farmer,
        FarmerDay,
        CityCenter,
        CityCenterPermit,
        Land,
        FoodDay,
        CityCenterDay,

    };

    const std::unordered_map<InventoryType, std::string> InventoryTypeToString = {
        {InventoryType::BuildingPermit, "BuildingPermit"},
        {InventoryType::Population, "Population"},
        {InventoryType::Food, "Food"},
        {InventoryType::Building, "Building"},
        {InventoryType::WaterSource, "WaterSource"},

    };

};
