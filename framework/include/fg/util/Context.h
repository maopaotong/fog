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
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string>
namespace fog
{
    template <typename T>
    class Context
    {
    private:
    public:        
        static T* ctxObj;

        static T* get()
        {
            return ctxObj;
        }
        
        static T* exchange(T* obj)
        {
            return std::exchange<T*>(ctxObj, obj);
        }

        static void set(T* obj)
        {
            ctxObj = obj;
        }

        static T* unset()
        {
            return std::exchange<T*>(ctxObj, nullptr);
        }

        template <typename... Args, typename F>
        static void runWithContext(T* ctxObj, F &&func, Args... args)
        {
            T* previousCtxObj = exchange(ctxObj);
            func(args...);
            set(previousCtxObj);
        }
    };
};
