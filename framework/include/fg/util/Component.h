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

    template <typename T>
    struct always_false : std::false_type
    {
    };

    template <typename T>
    struct ConstructorTraits;

    template <typename R, typename... Args>
    struct ConstructorTraits<R(Args...)>
    {
        using Type = R;
        using ArgsTuple = std::tuple<Args...>;
        static constexpr size_t arity = sizeof...(Args);
    };

    template <typename T, typename = void>
    struct hasInject : std::false_type
    {
    };

    template <typename T>
    struct hasInject<T, std::void_t<typename T::Inject>> : std::true_type
    {
    };

    struct Component
    {

        struct Injector
        {
        private:
        public:
            Injector()
            {
            }
            std::unordered_map<std::type_index, std::function<std::any()>> factories;

            std::function<std::any(std::type_index)> defaultBind;

            template <typename T>
            void bind()
            {
                bind<T, T>();
            }

            template <typename T>
            void upbind()
            {
                factories.erase(typeid(T));
            }

            template <typename T, typename F>
            void bind(F &&func)
            {
                std::type_index tid = typeid(T);
                if (factories.find(tid) != factories.end())
                {
                    throw std::runtime_error("cannot re-bind,unbind first.");
                }
                factories[tid] = [func]()
                {
                    return std::make_any<T *>(func());
                };
            }

            template <typename T, typename Imp>
            void bind()
            {
                static_assert(hasInject<Imp>::value, "macro INJECT missing or the INJECT constructor is not visible.");
                static_assert(!std::is_abstract<Imp>::value);
                bind<T>([this]() -> T *
                        { T *obj = getSingleton<Imp>(); 
                        return obj; });
            }
            template <typename T>
            T *get()
            {
                return std::any_cast<T *>(get(typeid(T)));
            }

            std::any get(std::type_index tid)
            {
                auto it = factories.find(tid);
                if (it == factories.end())
                {
                    if (defaultBind)
                    {
                        // call default function.
                        return defaultBind(tid);
                    }
                    throw std::runtime_error("must bond before get the instance by type.");
                }
                return (it->second)();
            }

            template <typename T>
            bool hasBind()
            {
                auto it = factories.find(typeid(T));
                return it != factories.end();
            }

            template <typename F>
            void setDefaultBind(F &&func)
            {
                this->defaultBind = func;
            }

            Injector &operator=(const Injector &injector)
            {
                this->factories = injector.factories;
                this->defaultBind = injector.defaultBind;
                return *this;
            }

        private:
            template <typename T>
            T *getSingleton()
            {
                static T *instance = createInstance<T>();
                return (instance);
            }

            template <typename T>
            typename std::enable_if<std::is_abstract<T>::value, T *>::type createInstance()
            {
                static_assert(always_false<T>::value, "abstract type & no injected impl class.");
            }

            template <typename T>
            typename std::enable_if<!std::is_abstract<T>::value && !hasInject<T>::value, T *>::type createInstance()
            {
                return new T(); //
            }

            template <typename T>
            typename std::enable_if<!std::is_abstract<T>::value && hasInject<T>::value, T *>::type createInstance()
            {

                using ArgsTuple = typename ConstructorTraits<T::Inject>::ArgsTuple;
                constexpr int N = ConstructorTraits<T::Inject>::arity;

                if constexpr (N == 0)
                {
                    return new T(); // same as no inject,injected the default constructor.
                }

                if constexpr (N == 1)
                {
                    using D0 = std::remove_reference_t<std::tuple_element_t<0, ArgsTuple>>;
                    static_assert(std::is_pointer_v<D0>, "not support non-pointer type as inject args");
                    using T0 = std::remove_pointer_t<D0>;
                    // find bind.

                    D0 d0 = get<T0>(); //

                    return new T(d0);
                }

                return createInstance<T, ArgsTuple>(std::make_index_sequence<N>{});

                // static_assert(N < 2, "todo more than 1 element in args list.");
            }
            template <typename Tuple, std::size_t... Is>
            static constexpr bool allArgsArePointersImpl(std::index_sequence<Is...>)
            {
                return (std::is_pointer_v<std::tuple_element_t<Is, Tuple>> && ...);
            }

            template <typename ArgsTuple>
            static constexpr bool allArgsArePointers = allArgsArePointersImpl<ArgsTuple>(
                std::make_index_sequence<std::tuple_size_v<std::decay_t<ArgsTuple>>>{});

            template <typename T, typename ArgsTuple, std::size_t... Is>
            T *createInstance(std::index_sequence<Is...>)
            {

                static_assert(allArgsArePointers<ArgsTuple>, "All inject arguments must be pointer types!");

                return new T(get<std::remove_pointer_t<std::tuple_element_t<Is, ArgsTuple>>>()...);
            }
        };
        //
    };
};
