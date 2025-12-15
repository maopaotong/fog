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
        struct Injector;

        using Interface = std::unordered_set<std::type_index>;
        using Object = std::unordered_map<std::type_index, std::function<std::any()>>;
        std::type_index typeId; // register the main type of the component.
        Object obj;             // cast funcs

        Component(std::type_index typeId, Object obj) : typeId(typeId), obj(obj) {};

        template <typename T>
        T *get()
        {
            std::type_index tid = typeid(T);
            auto it = obj.find(tid);
            if (it == obj.end())
            {
                return nullptr;
            }
            return std::any_cast<T *>((it->second)());
        }

        std::any get(std::type_index tid)
        {
            auto it = obj.find(tid);
            if (it == obj.end())
            {
                return nullptr;
            }
            return (it->second)();
        }

        template <typename F>
        void addType(std::type_index ifType, F &&func) // add a interface type for the component.
        {
            auto it = obj.find(typeid(T));
            if (it != obj.end())
            {
                throw std::runtime_error("type already exists, cannot add same type for a component.");
            }
            obj.emplace(ifType, func);
        }

        template <typename F>
        void forEachInterfaceType(F &&visit)
        {
            for (auto it = obj.begin(); it != obj.end(); it++)
            {
                visit(it->first);
            }
        }

        template <typename T>
        bool hasType()
        {
            std::type_index tid = typeid(T);
            return this->obj.find(tid) != this->obj.end();
        }

        // make methods
        template <typename F>
        static Component make(std::type_index tid, F &&create)
        {
            Object obj;
            obj.emplace(tid, create);
            return Component(tid, obj);
        }

        template <typename T, typename Imp, typename Tuple, std::size_t... Is>
        static Component makeImpl(std::function<std::any()> create, std::index_sequence<Is...>)
        {
            Object obj; // map from type id => function to get/crate the required type of value.
            ((registerInterface<T, Imp, Tuple, Is>(obj, create)), ...);
            return Component(typeid(T), obj);
        }

        template <typename T, typename Imp, typename Tuple, std::size_t I>
        static void registerInterface(Object &obj, std::function<std::any()> create)
        {
            using InterfaceType = std::tuple_element_t<I, Tuple>;
            std::type_index tid = typeid(InterfaceType);
            auto func = [create]() -> InterfaceType *
            {
                std::any impAny = create();
                T *main = std::any_cast<T *>(impAny);
                return static_cast<InterfaceType *>(main); // cast to type.
            };
            obj.emplace(tid, func);
        }

    public:
        struct Injector
        {
        private:
        public:
            Injector()
            {
            }
            std::unordered_map<std::type_index, Component> components;

            std::function<Component(std::type_index)> defaultCompFunc;

            template <typename T>
            void upbind()
            {
                components.erase(typeid(T));
            }

            template <typename T, typename F>
            void bindFunc(F &&objFunc)
            {
                bindComp(Component::make(typeid(T), objFunc));
            }
            void bindComp(Component comp)
            {
                if (components.find(comp.typeId) != components.end())
                {
                    throw std::runtime_error("type id already bond, cannot bind, you may need rebind method.");
                }
                components.emplace(comp.typeId, comp);
            }

            template <typename T>
            void bindImpl()
            {
                bindComp(makeByImpl<T>());
            }
            template <typename... T>
            void bindAllImpl()
            {
                ((bindImpl<T>()), ...);
            }
            template <typename T>
            void bindInstance(T *obj)
            {
                bindFunc<T>([obj]() -> T *
                            { return obj; });
            }

            template <typename T, typename Imp>
            void bindImpl()
            {
                bindComp(makeByImpl<T, Imp>());
            }
            template <typename T, typename Imp, typename T1>
            void bindImpl()
            {
                bindComp(makeByImpl<T, Imp, T1>());
            }

            template <typename T, typename Imp, typename T1, typename T2>
            void bindImpl()
            {
                bindComp(makeByImpl<T, Imp, T1, T2>());
            }

            template <typename T, typename OT>
            void bindMethod(T *(OT::*method)())
            {
                bindFunc<T>([this, method]()
                            {
                                OT *obj = this->get<OT>();
                                return (obj->*method)(); //
                            });
            }
            //

            template <typename T>
            Component makeByImpl()
            {
                return makeByImpl<T, T>();
            }

            template <typename T, typename Imp, typename... Adts>
            Component makeByImpl()
            {
                static_assert(hasInject<Imp>::value, "macro INJECT missing or the INJECT constructor is not visible.");
                static_assert(!std::is_abstract<Imp>::value);
                using TAdtsTuple = std::tuple<T, Adts...>;
                std::function<std::any()> func = [this]() -> std::any
                {
                    return std::make_any<T *>(this->getSingleton<Imp>());
                };
                return Component::makeImpl<T, Imp, TAdtsTuple>(func, std::make_index_sequence<std::tuple_size_v<std::decay_t<TAdtsTuple>>>{});
            }

            template <typename T>
            T *getSingleton()
            {
                static T *instance = createInstance<T>();
                return (instance);
            }

            //
            template <typename T>
            T *get()
            {
                return get<T, void>(typeid(T));
            }

            template <typename T>
            T *get(std::type_index tid)
            {
                return get<T, void>(tid);
            }

            //
            template <typename T, typename C>
            T *get()
            {
                return get<T, C>(typeid(T));
            }

            template <typename T, typename C>
            T *get(std::type_index tid)
            {
                typeid(C);
                return getComponent(tid).get<T>();
            }

            Component getComponent(std::type_index tid)
            {
                auto it = components.find(tid);
                if (it == components.end())
                {
                    if (defaultCompFunc)
                    {
                        // call default function.
                        return defaultCompFunc(tid);
                    }
                    throw std::runtime_error("must bond before get the instance by type.");
                }
                return it->second;
            }

            template <typename T>
            bool hasBind()
            {
                auto it = factories.find(typeid(T));
                return it != factories.end();
            }

            void bindDefault(Injector &pInjector)
            {
                this->defaultCompFunc = [&pInjector](std::type_index tid)
                { return pInjector.getComponent(tid); };
            }

            Injector &operator=(const Injector &injector)
            {
                this->components = injector.components;
                this->defaultCompFunc = injector.defaultCompFunc;
                return *this;
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

                // static_assert(allArgsArePointers<ArgsTuple>, "All inject arguments must be pointer types!");
                return new T(getPtrOrValue<std::tuple_element_t<Is, ArgsTuple>, T::Inject>()...);
            }

            template <typename Arg, typename C>
            typename std::enable_if_t<std::is_pointer_v<Arg>, Arg> getPtrOrValue() // return Arg or Arg *
            {
                return get<std::remove_pointer_t<Arg>, C>(); // pointer
            }

            template <typename Arg, typename C>
            typename std::enable_if_t<!std::is_pointer_v<Arg>, Arg> getPtrOrValue() // return Arg or Arg *
            {
                return *get<Arg, C>(); // pointer
            }
        };
        //
    };
};
