/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "Common.h"
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
        using AnyFunc = std::function<std::any()>;
        using AddonFunc = std::function<std::any(Injector&)>;
        using Usage = unsigned int;
        static constexpr Usage AsPtr = 1U << 0;
        static constexpr Usage AsVal = 1U << 2;
        static constexpr Usage AsStatic = 1U << 3;
        static constexpr Usage AsDynamic = 1U << 4;

        static constexpr Usage AsPtrStatic = AsPtr | AsStatic;
        static constexpr Usage AsPtrDynamic = AsPtr | AsDynamic;
        static constexpr Usage AsValStatic = AsVal | AsStatic;
        static constexpr Usage AsValDynamic = AsVal | AsDynamic;

        using Interface = std::unordered_set<std::type_index>;
        using Object = std::unordered_map<std::type_index, AnyFunc>;
        using Value = AnyFunc;

        std::type_index typeId; // register the main type of the component.
        Object obj;             // cast funcs
        Value value;

        Component(std::type_index typeId, Object obj, Value value) : typeId(typeId), obj(obj), value(value) {};

        template <typename T>
        T *getPtr()
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
        T getVal()
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

        std::any getPtr(std::type_index tid)
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
        static Component make(std::type_index tid, F &&createAsPtr, F2 &&createAsVal)
        {
            Object obj;
            obj.emplace(tid, createAsPtr);
            return Component(tid, obj, createAsVal);
        }

        template <typename T, typename Imp, typename Tuple, std::size_t... Is>
        static Component makeImpl(AnyFunc createAsPtr, AnyFunc createAsVal, std::index_sequence<Is...>)
        {
            Object obj; // map from type id => function to get/crate the required type of value.
            ((registerInterface<T, Imp, Tuple, Is>(obj, createAsPtr)), ...);

            return Component(typeid(T), obj, createAsVal);
        }

        template <typename T, typename Imp, typename Tuple, std::size_t I>
        static void registerInterface(Object &obj, AnyFunc createAsPtr)
        {
            using InterfaceType = std::tuple_element_t<I, Tuple>;
            std::type_index tid = typeid(InterfaceType);
            auto func = [createAsPtr]() -> InterfaceType *
            {
                std::any impAny = createAsPtr();
                T *main = std::any_cast<T *>(impAny);
                return static_cast<InterfaceType *>(main); // cast to type.
            };
            obj.emplace(tid, func);
        }

    public:
        struct Context
        {
            
            AnyFunc pFunc;
            AnyFunc vFunc;
            Context(AnyFunc pF, AnyFunc vF) : pFunc(pF), vFunc(vF)
            {
            }
        };




    public:
        struct Injector
        {

        private:
            std::unordered_map<std::type_index, std::stack<Context>> contexts;

            template <typename T, typename F>
            void runWithCtx(Context ctx, F &&func)
            {
                auto it = contexts.find(typeid(T));
                if (it == contexts.end())
                {
                    throw std::runtime_error("must bind type to context before run with the context of certain type.");
                }

                (*it).push(ctx);
                func();
                (*it).pop();
            }
                        
            template <typename T>
            T *getPtr(std::type_index tid)
            {
                return getComponent(tid).getPtr<T>();
            }
            
            template <typename T, typename Usage usg>
            AnyFunc getCtxFunc()
            {
                std::type_index tid = typeid(T);
                auto it = this->contexts.find(tid);
                if (it == this->contexts.end())
                {
                    throw std::runtime_error("impossible bug? context bind error?");
                }

                if (it->second.empty())
                {
                    throw std::runtime_error("context missing, push() first.");
                }
                AnyFunc func = usg & AsPtr ? it->second.top().pFunc : it->second.top().vFunc;
                if (!func)
                {
                    throw std::runtime_error("context usage function missing, usg:" + std::to_string(usg));
                }
                return func;
            }



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
           

            void bindComp(Component comp)
            {
                if (components.find(comp.typeId) != components.end())
                {
                    throw std::runtime_error("type id already bond, cannot bind, you may need rebind method.");
                }
                components.emplace(comp.typeId, comp);
            }

            template <typename T, typename F>
            void bindFuncAsPtr(F &&ptrFunc)
            {
                AnyFunc valueFunc = []() -> std::any
                { throw std::runtime_error("Usage as value not implemented for the type to be injected."); };
                bindComp(Component::make(typeid(T), ptrFunc, valueFunc));
            }

            template <typename T, typename F>
            void bindFuncAsVal(F &&valueFunc)
            {
                AnyFunc ptrFunc = []() -> std::any
                { throw std::runtime_error("Usage as value not implemented for the type to be injected."); };
                bindComp(Component::make(typeid(T), ptrFunc, valueFunc));
            }

            template <typename T, Usage usg = AsPtrStatic>
            void bindImpl()
            {
                bindComp(makeByImpl<usg, T, T>([](T &) -> void {}));
            }

            template <typename T>
            void bindImplAsVal()
            {
                bindComp(makeByImpl<AsValStatic, T, T>([](T &) -> void {}));
            }

            template <typename T, typename Imp>
            void bindImplAsVal()
            {
                bindComp(makeByImpl<AsValStatic, T, Imp>([](T &) -> void {}));
            }

            //
            template <typename T>
            void bindImpl(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<AsPtrStatic, T, T>(initF));
            }

            template <typename T>
            void bindImplAsVal(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<AsValStatic, T, T>(initF));
            }

            template <typename T, typename Imp>
            void bindImplAsVal(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<AsValStatic, T, Imp>(initF));
            }

            template <typename... T>
            void bindAllImplAsPtr()
            {
                ((bindImpl<T>([](T &) -> void {})), ...);
            }

            template <typename... T>
            void bindAllImplAsVal()
            {
                ((bindImplAsVal<T>([](T &) -> void {})), ...);
            }

            template <typename T>
            void bindPtr(T *obj)
            {
                bindFuncAsPtr<T>([obj]() -> T *
                                 { return obj; });
            }
            
            template <typename T>
            void push(T vCtx)
            {
                push<T>(nullptr, vCtx);
            }

            template <typename T>
            void pop()
            {
                auto it = this->contexts.find(typeid(T));
                if (it == this->contexts.end())
                {
                    throw std::runtime_error("context not bind to the type.");
                }
                (*it).second.pop();
            }

            template <typename T>
            void push(T *pCtx, T vCtx)
            {
                if (auto it = this->contexts.find(typeid(T)); it != this->contexts.end())
                {
                    AnyFunc vF = [&vCtx]() -> std::any
                    { return std::make_any<T>(vCtx); };

                    AnyFunc pF;
                    if (pCtx)
                    {
                        pF = [pCtx]() -> std::any
                        { return std::make_any<T *>(pCtx); };
                    }

                    it->second.push(Context(pF, vF));
                    return;
                }
                throw std::runtime_error("context not bind to the type.");
            }

            template <typename T>
            void bindCtx()
            {

                if (auto it = this->contexts.find(typeid(T)); it != this->contexts.end())
                {
                    throw std::runtime_error("already bind type to context, cannot re-bind.");
                }

                this->contexts.emplace(typeid(T), std::stack<Context>()); // initial the stack for the type id.

                auto ptrFunc = [this]()
                {
                    return getCtxFunc<T, AsPtr>()();
                };

                auto valFunc = [this]()
                {
                    return getCtxFunc<T, AsVal>()();
                };

                bindComp(Component::make(typeid(T), ptrFunc, valFunc));
            }

            template <typename T>
            void bindVal(T obj)
            {
                bindFuncAsVal<T>([obj]() -> T
                                 { return obj; });
            }
            //
            template <typename T, typename Imp, Usage usg = AsPtrStatic>
            void bindImpl()
            {
                bindComp(makeByImpl<usg, T, Imp>([](T &) -> void {}));
            }

            template <typename T, typename Imp, typename T1, Usage usg = AsPtrStatic>
            void bindImpl()
            {
                bindComp(makeByImpl<usg, T, Imp, T1>([](T &) -> void {}));
            }

            template <typename T, typename Imp, typename T1, typename T2, Usage usg = AsPtrStatic>
            void bindImpl()
            {
                bindComp(makeByImpl<usg, T, Imp, T1, T2>([](T &) -> void {}));
            }
            //
            template <typename T, typename Imp, Usage usg = AsPtrStatic>
            void bindImpl(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<usg, T, Imp>(initF));
            }

            template <typename T, typename Imp, typename T1, Usage usg = AsPtrStatic>
            void bindImpl(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<usg, T, Imp, T1>(initF));
            }

            template <typename T, typename Imp, typename T1, typename T2, Usage usg = AsPtrStatic>
            void bindImpl(std::function<void(T &)> initF)
            {
                bindComp(makeByImpl<usg, T, Imp, T1, T2>(initF));
            }
            //
            template <typename T, typename OT>
            void bindMethod(T *(OT::*method)())
            {
                bindFuncAsPtr<T>([this, method]()
                                 {
                                     OT *obj = this->getPtr<OT>();
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
                AnyFunc funcAsPtr;
                AnyFunc funcAsVal;
                makeFunctionForUsage<usg, T, Imp>(initF, funcAsPtr, funcAsVal);
                return Component::makeImpl<T, Imp, TAdtsTuple>(funcAsPtr, funcAsVal, std::make_index_sequence<std::tuple_size_v<std::decay_t<TAdtsTuple>>>{});
            }

            template <Usage usg, typename T, typename Imp>
            typename std::enable_if_t<usg & AsPtr, void> makeFunctionForUsage(std::function<void(T &)> initF, AnyFunc &funcAsPtr, AnyFunc &funcAsVal)
            {
                funcAsPtr = [this, initF]() -> std::any
                {
                    return std::make_any<T *>(this->getPtr<usg, Imp>(initF));
                };
            }

            template <Usage usg, typename T, typename Imp>
            typename std::enable_if_t<usg & AsVal, void> makeFunctionForUsage(std::function<void(T &)> initF, AnyFunc &funcAsPtr, AnyFunc &funcAsVal)
            {
                funcAsVal = [this, initF]() -> std::any
                {
                    return std::make_any<T>(this->getVal<usg, Imp>(initF));
                };
            }

            template <Usage usg, typename T>
            typename std::enable_if_t<usg & AsStatic, T *>
            getPtr(std::function<void(T &)> initF)
            {
                static std::once_flag flag;
                static T *instance = createInstance<AsPtr, T>();
                std::call_once(flag, [initF]()
                               { initF(*instance); });
                return (instance);
            }

            template <Usage usg, typename T>
            typename std::enable_if_t<usg & AsDynamic, T *>
            getPtr(std::function<void(T &)> initF)
            {
                T *ptr = createInstance<AsPtr, T>();
                initF(*ptr);
                return ptr;
            }

            template <Usage usg, typename T>
            typename std::enable_if_t<usg & AsStatic, T>
            getVal(std::function<void(T &)> initF)
            {
                static std::once_flag flag;
                static T instance = createInstance<AsVal, T>();
                std::call_once(flag, [initF]()
                               { initF(instance); });
                return (instance);
            }

            template <Usage usg, typename T>
            typename std::enable_if_t<usg & AsDynamic, T>
            getVal(std::function<void(T &)> initF)
            {
                T ret = createInstance<AsVal, T>();
                initF(ret);
                return ret;
            }

            //
            template <typename T>
            T *getPtr()
            {
                return getComponent(typeid(T)).getPtr<T>();
            }

            template <typename T>
            void push(std::function<T *()> ptrCtx, std::function<T()> valCtx)
            {
                if (auto it = this->contexts.find(typeid(T)); it != this->contexts.end())
                {
                    AnyFunc pFunc;
                    if (ptrCtx)
                    {
                        pFunc = [ptrCtx]()
                        { return std::make_any<T *>(ptrCtx()); };
                    }

                    AnyFunc vFunc;
                    if (valCtx)
                    {
                        vFunc = [valCtx]()
                        { return std::make_any<T>(valCtx()); };
                    }

                    it->second.push(Context(pFunc, vFunc));
                    return;
                }
                throw std::runtime_error("context stack missing, you need bind type to context first.");
            }

            template <typename T>
            T getVal()
            {
                return getComponent(typeid(T)).getVal<T>();
            }
            template <typename T>
            T getVal(std::type_index tid)
            {
                return getComponent(tid).getVal<T>();
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
            typename std::enable_if_t<usg & AsPtr && std::is_abstract_v<T>, T *> createInstance()
            {
                static_assert(always_false<T>::value, "abstract type & no injected impl class registered.");
            }
            // abstract as Value
            template <Usage usg, typename T>
            typename std::enable_if_t<usg & AsPtr && std::is_abstract_v<T>, T> createInstance()
            {
                static_assert(always_false<T>::value, "abstract type & no injected impl class registered.");
            }

            // concrete && !hasInject && as Pointer
            template <Usage usg, typename T>
            typename std::enable_if_t<usg & AsPtr && !std::is_abstract_v<T> && !hasInject<T>::value, T /*AsPtr*/ *> createInstance()
            {
                T *ret = new T{};
                return ret;
            }

            // concrete && !hasInject && as Value
            template <Usage usg, typename T>
            typename std::enable_if_t<usg & AsVal && !std::is_abstract_v<T> && !hasInject<T>::value, T /*AsVal*/> createInstance()
            {
                T ret = T{}; //
                return ret;
            }

            // concrete && hasInject && as Pointer
            template <Usage usg, typename T>
            typename std::enable_if_t<usg & AsPtr && !std::is_abstract_v<T> && hasInject<T>::value, T /*AsPtr*/ *> createInstance()
            {

                using ArgsTuple = typename ConstructorTraits<T::Inject>::ArgsTuple;
                constexpr int N = ConstructorTraits<T::Inject>::arity;
                return createInstanceByConstructor<usg, T, ArgsTuple>(std::make_index_sequence<N>{});

                // static_assert(N < 2, "todo more than 1 element in args list.");
            }

            // concrete && hasInject && as Pointer
            template <Usage usg, typename T>
            typename std::enable_if_t<usg & AsVal && !std::is_abstract_v<T> && hasInject<T>::value, T /*AsVal*/> createInstance()
            {

                using ArgsTuple = typename ConstructorTraits<T::Inject>::ArgsTuple;
                constexpr int N = ConstructorTraits<T::Inject>::arity;
                return createInstanceByConstructor<usg, T, ArgsTuple>(std::make_index_sequence<N>{});

                // static_assert(N < 2, "todo more than 1 element in args list.");
            }

            // C<1>:As Pointer
            template <Usage usg, typename T, typename ArgsTuple, std::size_t... Is>
            typename std::enable_if_t<usg & AsPtr, T *>
            createInstanceByConstructor(std::index_sequence<Is...>)
            {
                // static_assert(allArgsArePointers<ArgsTuple>, "All inject arguments must be pointer types!");
                T *ret = new T(getPtrOrValue<std::tuple_element_t<Is, ArgsTuple>>()...);
                return ret;
            }
            // C<2>:As Value
            template <Usage usg, typename T, typename ArgsTuple, std::size_t... Is>
            typename std::enable_if_t<usg & AsVal, T>
            createInstanceByConstructor(std::index_sequence<Is...>)
            {

                // static_assert(allArgsArePointers<ArgsTuple>, "All inject arguments must be pointer types!");

                T ret = T(getPtrOrValue<std::tuple_element_t<Is, ArgsTuple>>()...);
                return ret;
            }

            // G<1>
            template <typename Arg>
            typename std::enable_if_t<std::is_pointer_v<Arg>, Arg> getPtrOrValue() // return Arg or Arg *
            {
                return getPtr<std::remove_pointer_t<Arg>>(); // pointer
            }

            // G<2>
            template <typename Arg>
            typename std::enable_if_t<!std::is_pointer_v<Arg>, Arg> getPtrOrValue() // return Arg or Arg *
            {
                return getVal<Arg>(); // pointer
            }
        };
        //
    };
};
