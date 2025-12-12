/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
namespace fog
{
class InputState
{
public:
    virtual bool isLeft() { return false; };
    virtual bool isRight() { return false; };
    virtual bool isFront() { return false; };
    virtual bool isBack() { return false; };
    virtual bool isUp() { return false; };
    virtual bool isDown() { return false; };

    bool isMoving()
    {
        return isLeft() || isRight() || isFront() || isBack();
    }
};
};
