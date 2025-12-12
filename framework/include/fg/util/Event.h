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
#include "Common.h"

namespace fog
{

    class Event
    {
    public:
        class Bus
        {
        public:
            template <typename... Args, typename F>
            void subscribe(F &&func)
            {
                //
                getCallbacks<Args...>().emplace_back(std::forward<F>(func));
            }

            template <typename... Args>
            void emit(Args... args)
            {
                for (auto &cb : getCallbacks<Args...>())
                {
                    cb(args...);
                }
            }

        private:
            struct CallbackBase
            {
                virtual ~CallbackBase() = default;
            };

            template <typename... Args>
            struct CallbackList : CallbackBase
            {
                std::vector<std::function<void(Args...)>> callbacks;
            };

            std::unordered_map<std::type_index, std::unique_ptr<CallbackBase>> registry;

            template <typename... Args>
            std::vector<std::function<void(Args...)>> &getCallbacks()
            {
                std::type_index ti = std::type_index(typeid(std::tuple<Args...>));
                auto it = registry.find(ti);
                if (it == registry.end())
                {
                    auto list = std::make_unique<CallbackList<Args...>>();
                    auto &ref = list->callbacks;
                    registry[ti] = std::move(list);
                    return ref;
                }
                return static_cast<CallbackList<Args...> *>(it->second.get())->callbacks;
            }
        };
    };
};
