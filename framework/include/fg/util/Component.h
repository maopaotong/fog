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
        using Usage = unsigned int;
        static constexpr Usage AsPointer = 1;
        static constexpr Usage AsValue = 1 << 1;

        using Interface = std::unordered_set<std::type_index>;
        using Object = std::unordered_map<std::type_index, std::function<std::any()>>;
        using Value = std::function<std::any()>;

        std::type_index typeId; // register the main type of the component.
        Object obj;             // cast funcs
        Value value;

        Component(std::type_index typeId, Object obj, Value value) : typeId(typeId), obj(obj), value(value) {};

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

        template <typename T>
        T getValue()
        {
            if (!value)
            {
                throw std::runtime_error("cannot get value , no value function registered for component as a value. ");
            }
            std::any v = value();
            if (v.type() != typeid(T))
            {
                throw std::runtime_error("cannot cast value , value function returned a value with other type. ");
            }
            return std::any_cast<T>(v);
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
        template <typename F, typename F2>
        static Component make(std::type_index tid, F &&createAsPointer, F2 &&createAsValue)
        {
            Object obj;
            obj.emplace(tid, createAsPointer);
            return Component(tid, obj, createAsValue);
        }

        template <typename T, typename Imp, typename Tuple, std::size_t... Is>
        static Component makeImpl(std::function<std::any()> createAsPointer, std::function<std::any()> createAsValue, std::index_sequence<Is...>)
        {
            Object obj; // map from type id => function to get/crate the required type of value.
            ((registerInterface<T, Imp, Tuple, Is>(obj, createAsPointer)), ...);

            return Component(typeid(T), obj, createAsValue);
        }

        template <typename T, typename Imp, typename Tuple, std::size_t I>
        static void registerInterface(Object &obj, std::function<std::any()> createAsPointer)
        {
            using InterfaceType = std::tuple_element_t<I, Tuple>;
            std::type_index tid = typeid(InterfaceType);
            auto func = [createAsPointer]() -> InterfaceType *
            {
                std::any impAny = createAsPointer();
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
            void bindFuncAsPtr(F &&ptrFunc)
            {
                std::function<std::any()> valueFunc = []() -> std::any
                { throw std::runtime_error("Usage as value not implemented for the type to be injected."); };
                bindComp(Component::make(typeid(T), ptrFunc, valueFunc));
            }

            template <typename T, typename F>
            void bindFuncAsValue(F &&valueFunc)
            {
                std::function<std::any()> ptrFunc = []() -> std::any
                { throw std::runtime_error("Usage as value not implemented for the type to be injected."); };
                bindComp(Component::make(typeid(T), ptrFunc, valueFunc));
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
            void bindImplAsPtr()
            {
                bindComp(makeByImpl<AsPointer, T, T>([](T &) -> void {}));
            }

            template <typename T>
            void bindImplAsValue()
            {
                bindComp(makeByImpl<AsValue, T, T>([](T &) -> void {}));
            }

            template <typename T, typename Imp>
            void bindImplAsValue()
            {
                bindComp(makeByImpl<AsValue, T, Imp>([](T &) -> void {}));
            }

            //
            template <typename T>
            void bindImplPtr(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<AsPointer, T, T>(initF));
            }

            template <typename T>
            void bindImplAsValue(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<AsValue, T, T>(initF));
            }

            template <typename T, typename Imp>
            void bindImplAsValue(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<AsValue, T, Imp>(initF));
            }

            template <typename... T>
            void bindAllImplAsPtr()
            {
                ((bindImplPtr<T>([](T &) -> void {})), ...);
            }

            template <typename... T>
            void bindAllImplAsValue()
            {
                ((bindImplAsValue<T>([](T &) -> void {})), ...);
            }

            template <typename T>
            void bindInstance(T *obj)
            {
                bindFuncAsPtr<AsPointer, T>([obj]() -> T *
                                       { return obj; });
            }

            template <typename T>
            void bindValue(T obj)
            {
                bindFuncAsValue<T>([obj]() -> T
                                     { return obj; });
            }
            //
            template <typename T, typename Imp>
            void bindImplAsPtr()
            {
                bindComp(makeByImpl<AsPointer, T, Imp>([](T &) -> void {}));
            }

            template <typename T, typename Imp, typename T1>
            void bindImplAsPtr()
            {
                bindComp(makeByImpl<AsPointer, T, Imp, T1>([](T &) -> void {}));
            }

            template <typename T, typename Imp, typename T1, typename T2>
            void bindImplAsPtr()
            {
                bindComp(makeByImpl<AsPointer, T, Imp, T1, T2>([](T &) -> void {}));
            }
            //
            template <typename T, typename Imp>
            void bindImplPtr(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<AsPointer, T, Imp>(initF));
            }

            template <typename T, typename Imp, typename T1>
            void bindImplPtr(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<AsPointer, T, Imp, T1>(initF));
            }

            template <typename T, typename Imp, typename T1, typename T2>
            void bindImplPtr(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<AsPointer, T, Imp, T1, T2>(initF));
            }
            //
            template <typename T, typename OT>
            void bindMethod(T *(OT::*method)())
            {
                bindFuncAsPtr<T>([this, method]()
                            {
                                OT *obj = this->get<OT>();
                                return (obj->*method)(); //
                            });
            }
            //

            template <Usage usg, typename T, typename Imp, typename... Adts>
            Component makeByImpl(std::function<void(T &)> initF)
            {
                static_assert(hasInject<Imp>::value, "macro INJECT missing or the INJECT constructor is not visible.");
                static_assert(!std::is_abstract<Imp>::value);
                using TAdtsTuple = std::tuple<T, Adts...>;
                std::function<std::any()> funcAsPointer;
                std::function<std::any()> funcAsValue;
                makeFunctionForUsage<usg, T, Imp>(initF, funcAsPointer, funcAsValue);
                return Component::makeImpl<T, Imp, TAdtsTuple>(funcAsPointer, funcAsValue, std::make_index_sequence<std::tuple_size_v<std::decay_t<TAdtsTuple>>>{});
            }

            template <Usage usg, typename T, typename Imp>
            typename std::enable_if_t<usg == AsPointer, void> makeFunctionForUsage(std::function<void(T &)> initF, std::function<std::any()> &funcAsPointer, std::function<std::any()> &funcAsValue)
            {
                funcAsPointer = [this, initF]() -> std::any
                {
                    return std::make_any<T *>(this->getSingletonPointer<Imp>(initF));
                };
            }

            template <Usage usg, typename T, typename Imp>
            typename std::enable_if_t<usg == AsValue, void> makeFunctionForUsage(std::function<void(T &)> initF, std::function<std::any()> &funcAsPointer, std::function<std::any()> &funcAsValue)
            {
                funcAsValue = [this, initF]() -> std::any
                {
                    return std::make_any<T>(this->getSingletonValue<Imp>(initF));
                };
            }

            template <typename T>
            T *getSingletonPointer(std::function<void(T &)> initF)
            {
                static T *instance = createInstance<AsPointer, T>(initF);
                return (instance);
            }

            template <typename T>
            T getSingletonValue(std::function<void(T &)> initF)
            {
                static T instance = createInstance<AsValue, T>(initF);
                return (instance);
            }

            //
            template <typename T>
            T *get()
            {
                return getComponent(typeid(T)).get<T>();
            }

            template <typename T>
            T getValue()
            {
                return getComponent(typeid(T)).getValue<T>();
            }

            template <typename T>
            T *get(std::type_index tid)
            {
                return getComponent(tid).get<T>();
            }

            template <typename T>
            T getValue(std::type_index tid)
            {
                return getComponent(tid).getValue<T>();
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
                    throw std::runtime_error("must bind before get the instance by type from Component::Injector.");
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

            // abstract as Pointer.
            template <Usage usg, typename T>
            typename std::enable_if_t<usg == AsPointer && std::is_abstract_v<T>, T *> createInstance(std::function<void(T &)> initF)
            {
                static_assert(always_false<T>::value, "abstract type & no injected impl class registered.");
            }
            // abstract as Value
            template <Usage usg, typename T>
            typename std::enable_if_t<usg == AsPointer && std::is_abstract_v<T>, T> createInstance(std::function<void(T &)> initF)
            {
                static_assert(always_false<T>::value, "abstract type & no injected impl class registered.");
            }

            // concrete && !hasInject && as Pointer
            template <Usage usg, typename T>
            typename std::enable_if_t<usg == AsPointer && !std::is_abstract_v<T> && !hasInject<T>::value, T /*AsPointer*/ *> createInstance(std::function<void(T &)> initF)
            {
                T *ret = new T{}; //
                initF(*ret);
                return ret;
            }

            // concrete && !hasInject && as Value
            template <Usage usg, typename T>
            typename std::enable_if_t<usg == AsValue && !std::is_abstract_v<T> && !hasInject<T>::value, T /*AsValue*/> createInstance(std::function<void(T &)> initF)
            {
                T ret = T{}; //
                initF(ret);
                return ret;
            }

            // concrete && hasInject && as Pointer
            template <Usage usg, typename T>
            typename std::enable_if_t<usg == AsPointer && !std::is_abstract_v<T> && hasInject<T>::value, T /*AsPointer*/ *> createInstance(std::function<void(T &)> initF)
            {

                using ArgsTuple = typename ConstructorTraits<T::Inject>::ArgsTuple;
                constexpr int N = ConstructorTraits<T::Inject>::arity;
                return createInstanceByConstructor<usg, T, ArgsTuple>(std::make_index_sequence<N>{}, initF);

                // static_assert(N < 2, "todo more than 1 element in args list.");
            }

            // concrete && hasInject && as Pointer
            template <Usage usg, typename T>
            typename std::enable_if_t<usg == AsValue && !std::is_abstract_v<T> && hasInject<T>::value, T /*AsValue*/> createInstance(std::function<void(T &)> initF)
            {

                using ArgsTuple = typename ConstructorTraits<T::Inject>::ArgsTuple;
                constexpr int N = ConstructorTraits<T::Inject>::arity;
                return createInstanceByConstructor<usg, T, ArgsTuple>(std::make_index_sequence<N>{}, initF);

                // static_assert(N < 2, "todo more than 1 element in args list.");
            }

            // C<1>:As Pointer
            template <Usage usg, typename T, typename ArgsTuple, std::size_t... Is>
            typename std::enable_if_t<usg == AsPointer, T *>
            createInstanceByConstructor(std::index_sequence<Is...>, std::function<void(T &)> initF)
            {
                // static_assert(allArgsArePointers<ArgsTuple>, "All inject arguments must be pointer types!");
                T *ret = new T(getPtrOrValue<std::tuple_element_t<Is, ArgsTuple>>()...);
                initF(*ret);
                return ret;
            }
            // C<2>:As Value
            template <Usage usg, typename T, typename ArgsTuple, std::size_t... Is>
            typename std::enable_if_t<usg == AsValue, T>
            createInstanceByConstructor(std::index_sequence<Is...>, std::function<void(T &)> initF)
            {

                // static_assert(allArgsArePointers<ArgsTuple>, "All inject arguments must be pointer types!");

                T ret = T(getPtrOrValue<std::tuple_element_t<Is, ArgsTuple>>()...);
                initF(ret);
                return ret;
            }

            // G<1>
            template <typename Arg>
            typename std::enable_if_t<std::is_pointer_v<Arg>, Arg> getPtrOrValue() // return Arg or Arg *
            {
                return get<std::remove_pointer_t<Arg>>(); // pointer
            }

            // G<2>
            template <typename Arg>
            typename std::enable_if_t<!std::is_pointer_v<Arg>, Arg> getPtrOrValue() // return Arg or Arg *
            {
                return getValue<Arg>(); // pointer
            }
        };
        //
    };
};
