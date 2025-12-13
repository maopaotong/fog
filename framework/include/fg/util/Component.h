/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string>
#include <tuple>
#include <fg/util/Common.h>

#define INJECT(Sig)     \
    using Inject = Sig; \
    Sig

namespace fog
{

    struct Component
    {
        void *obj;

        Component(void *obj) : obj(obj)
        {
        }
        Component(Component &&comp)
        { // move constructor.
            this->obj = comp.obj;
            comp.obj = nullptr; // move
        }

        template <typename T>
        T *get()
        {
            return static_cast<T *>(obj);
        }
    };

    template <typename T>
    struct FunctionTraits;

    template <typename R, typename... Args>
    struct FunctionTraits<R(Args...)>
    {
        using Type = R;
        using ArgsTuple = std::tuple<Args...>;
        static constexpr size_t arity = sizeof...(Args);
    };

    template <typename T, typename... Args>
    struct ConstructorSignature
    {
        using Class = T;
        using ArgsTuple = std::tuple<Args...>;
    };

    template <typename T, typename = void>
    struct GetConstructor;

    template <typename T>
    struct GetConstructor<T, std::void_t<typename T::Inject>>
    {
        using type = typename T::Inject;
    };

    template <typename T, typename = void>
    struct hasInjectImpl : std::false_type
    {
    };

    template <typename T>
    struct hasInjectImpl<T, std::void_t<typename T::Inject>> : std::true_type
    {
    };

    template <typename T>
    static constexpr bool injectable = hasInjectImpl<T>::value;

    struct Injector
    {
        std::unordered_map<std::type_index, std::function<Component()>> factories;

        template <typename T, typename Imp>
        void bind()
        {
            static_assert(injectable<Imp>, "macro INJECT missing.");
            factories[typeid(T)] = [this]() -> Component
            {
                T *obj = createInstance<Imp>();
                return Component(obj);
            };
        }

        template <typename T>
        T *get()
        {
            std::type_index tid = typeid(T);
            auto it = factories.find(tid);
            if (it == factories.end())
            {
                if constexpr (std::is_abstract_v<T>)
                {
                    static_assert(false, "abstract type must be explicitly bound via bind<Interface, Impl>()");
                }
                else
                {
                    return createInstance<T>(); // concrete class, OK to fallback
                }
            }

            auto func = factories[typeid(T)];

            Component comp = func();
            return comp.get<T>();
        }

        template <typename T>
        static T *createInstance()
        {
            if constexpr (!injectable<T>) // no inject type found, plain object, new a plain object directly.
            {
                return new T();
            }
            else
            { // using
                using Sig = typename T::Inject;
                using Traits = FunctionTraits<Sig>;
                using ArgsTuple = typename Traits::ArgsTuple;
                constexpr int N = Traits::arity;

                if constexpr (N == 0)
                {
                    return new T();
                }

                if constexpr (N == 1)
                {
                    using D0 = std::remove_reference_t<std::tuple_element_t<0, ArgsTuple>>;
                    static_assert(std::is_pointer_v<D0>, "not support non-pointer type as inject args");
                    using T0 = std::remove_pointer_t<D0>;
                    // find bind.

                    D0 d0 = nullptr; // get<T0>(); //

                    return new T(d0);
                }

                static_assert(N < 2, "todo more than 1 element in args list.");
            }
        }

        template <typename T>
        static T *getSingleton()
        {
            static auto instance = createInstance<T>();
            return instance;
        }
    };
    //

};
