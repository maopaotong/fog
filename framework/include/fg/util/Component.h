/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once

#include "Common.h"
#include "Options.h"

#define INJECT(Sig)     \
    using Inject = Sig; \
    Sig

#define SELF(T) \
    using Self = T;

#define GROUP(G) \
    static inline std::string Group{G};

#define MEMBER(ftype, mname)                                                                   \
    ftype mname;                                                                              \
    struct AutoRegisteredMember_##mname                                                        \
    {                                                                                         \
        AutoRegisteredMember_##mname()                                                         \
        {                                                                                     \
            AutoRegisteredObjects::getInstance().addMember<Self, ftype>(#mname, &Self::mname); \
        };                                                                                    \
    };                                                                                        \
    static inline AutoRegisteredMember_##mname autoRegisteredMember_##mname{};

#define INIT(mname)                                                           \
    struct AutoRegisteredInit_##mname                                         \
    {                                                                         \
        AutoRegisteredInit_##mname()                                          \
        {                                                                     \
            AutoRegisteredObjects::getInstance().addInit<Self>(&Self::mname); \
        };                                                                    \
    };                                                                        \
    static inline AutoRegisteredInit_##mname autoRegisteredInit_##mname{};    \
    void mname

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

    template <typename T, typename = void>
    struct hasGroup : std::false_type
    {
    };

    template <typename T>
    struct hasGroup<T, std::void_t<decltype(T::Group)>> : std::true_type
    {
    };
    template <typename T, typename = void>
    struct hasVoidInit : std::false_type
    {
    };

    template <typename T>
    struct hasVoidInit<T, std::void_t<
                              decltype(std::declval<T>().init())>> : std::is_same<decltype(std::declval<T>().init()),
                                                                                  void>
    {
    };

    struct AutoRegisteredObjects
    {
        struct FieldInfo
        {
            bool asPtr;
            std::type_index pType;
            std::type_index vType;
            std::function<void(std::any, std::any)> setter;
            FieldInfo(bool asPtr, std::type_index pType, std::type_index vType, //
                      std::function<void(std::any, std::any)> setter) :         //
                                                                        asPtr(asPtr), pType(pType), vType(vType), setter(setter)
            {
            }
        };
        struct MethodInfo
        {
            std::function<void(std::any)> method;
            MethodInfo(std::function<void(std::any)> method) : method(method)
            {
            }
        };
        struct ObjectInfo
        {
            std::unordered_map<std::string, FieldInfo> fields;
            std::vector<MethodInfo> inits;
        };

        std::unordered_map<std::type_index, ObjectInfo> objects;

        template <typename T, typename F>
        void addMember(const std::string &fieldName, F T::*fieldPtr)
        {
            auto &objInfo = objects[std::type_index(typeid(T))];
            bool isPtr = std::is_pointer_v<F>;

            std::type_index vType = isPtr ? typeid(std::remove_pointer_t<F>) : typeid(F);
            std::type_index pType = isPtr ? typeid(F) : typeid(F *);
            FieldInfo fieldInfo(isPtr, pType, vType, [fieldPtr](std::any obj, std::any value)
                                {
                                    T *tObj = std::any_cast<T *>(obj);
                                    F val = std::any_cast<F>(value);
                                    tObj->*fieldPtr = val; //
                                });

            objInfo.fields.emplace(fieldName, fieldInfo);
        }

        template <typename T, typename F>
        void addInit(F T::*init)
        {
            auto &objInfo = objects[std::type_index(typeid(T))];

            MethodInfo methodInfo([init](std::any obj)
                                  {
                                      T *tObj = std::any_cast<T *>(obj);
                                      (tObj->*init)(); //
                                  });
            if (objInfo.inits.size() > 0)
            {
                throw std::runtime_error("only support single init method, there are already one registered.");
            }
            objInfo.inits.emplace_back(methodInfo);
        }

        static AutoRegisteredObjects &getInstance()
        {
            static AutoRegisteredObjects instance;
            return instance;
        }
    };

    template <typename T, typename C>
    struct ArgOfConstructor
    {
        using UsageFunc = std::function<std::any()>;
        UsageFunc valFunc;
        UsageFunc ptrFunc;
        ArgOfConstructor(UsageFunc ptrFunc, UsageFunc valFunc) : ptrFunc(ptrFunc), valFunc(valFunc)
        {
        }
    };

    struct Component
    {
        struct Injector;
        using AnyFunc = std::function<std::any()>;
        using AddonFunc = std::function<std::any(Injector &)>;
        using FieldsFunc = std::function<bool(std::type_index, std::string, std::any &)>;
        using InitFunc = std::function<bool(std::any)>;

        using Usage = unsigned int;
        static constexpr Usage AsPtr = 1U << 0;
        static constexpr Usage AsVal = 1U << 1;
        static constexpr Usage AsStatic = 1U << 2;
        static constexpr Usage AsDynamic = 1U << 3;
        static constexpr Usage AsStaticFirst = 1U << 4;  // trying to use static and dynamic if static unsupported.
        static constexpr Usage AsDynamicFirst = 1U << 5; // trying dynamic first, then static.

        static constexpr Usage AsPtrStatic = AsPtr | AsStatic;
        static constexpr Usage AsPtrDynamic = AsPtr | AsDynamic;
        static constexpr Usage AsValStatic = AsVal | AsStatic;
        static constexpr Usage AsValDynamic = AsVal | AsDynamic;
        static constexpr Usage AsPtrOrValStatic = AsPtrStatic | AsValStatic;

        using UsageFunc = std::function<std::any()>;
        using Interface = std::unordered_set<std::type_index>;
        using Object = std::unordered_map<std::type_index, UsageFunc>;
        using Value = UsageFunc;
        using Fields = FieldsFunc;
        using Init = InitFunc;

        std::type_index typeId; // register the main type of the component.
        Object objS;            // cast funcs
        Object objD;            // cast funcs
        Value valueStatic;
        Value valueDynamic;

        Fields fields;
        Init init;

        Component(std::type_index typeId, Object objS, Object objD, Value valueS, Value valueD, Fields fields, Init init)
            : typeId(typeId), objS(objS), objD(objD), valueStatic(valueS), valueDynamic(valueD), fields(fields), init(init) {};

        template <typename T>
        T *getPtr(Usage usgR)
        {
            std::any aV = getPtr(usgR, typeid(T));
            return std::any_cast<T *>(aV);
        }

        std::any getPtr(Usage usgR, std::type_index tid)
        {
            UsageFunc func = getPtrFunc(usgR, tid);
            return func();
        }

        template <typename T>
        T getVal(Usage usgR)
        {
            std::any aV = getVal(usgR, typeid(T));
            return std::any_cast<T>(aV);
        }

        std::any getVal(Usage usgR, std::type_index tid)
        {
            Value func = getValFunc(usgR);
            return func();
        }

        UsageFunc getValFunc(Usage usgR)
        {
            UsageFunc func;
            if (usgR & AsStaticFirst)
            {
                func = valueStatic ? valueStatic : valueDynamic;
            }
            else if (usgR & AsDynamicFirst)
            {
                func = valueDynamic ? valueDynamic : valueStatic;
            }
            else if (usgR & AsStatic)
            {
                func = valueStatic;
            }
            else if (usgR & AsDynamic)
            {
                func = valueDynamic;
            }
            if (!func)
            {
                throw std::runtime_error("no value function registed for the usage required.");
            }
            return func;
        }

        UsageFunc getPtrFunc(Usage usgR, std::type_index tid)
        {
            std::function<void()> f;

            UsageFunc func;
            if (usgR & AsStaticFirst)
            {
                func = getPtrFuncStatic(tid);
                if (!func)
                {
                    func = getPtrFuncDynamic(tid);
                }
            }
            else if (usgR & AsDynamicFirst)
            {
                func = getPtrFuncDynamic(tid);
                if (!func)
                {
                    func = getPtrFuncStatic(tid);
                }
            }
            else if (usgR & AsStatic)
            {
                func = getPtrFuncStatic(tid);
            }
            else if (usgR & AsDynamic)
            {
                func = getPtrFuncDynamic(tid);
            }

            if (!func)
            {
                throw std::runtime_error("no ptr func found for the type and usage required.");
            }
            return func;
        }

        UsageFunc getPtrFuncStatic(std::type_index tid)
        {
            if (auto it = objS.find(tid); it != objS.end())
            {
                return it->second;
            }
            return {};
        }

        UsageFunc getPtrFuncDynamic(std::type_index tid)
        {
            if (auto it = objD.find(tid); it != objD.end())
            {
                return it->second;
            }
            return {};
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
        static Component make(std::type_index tid, F &&createAsPtrS, F2 &&createAsValS)
        {
            return make(tid, createAsPtrS, createAsValS, {}, {}, {}, {});
        }

        static Component make(std::type_index tid, UsageFunc createAsPtrS, UsageFunc createAsValS, UsageFunc createAsPtrD, UsageFunc createAsValD,
                              FieldsFunc fields, InitFunc init)
        {
            Object objS;
            objS.emplace(tid, createAsPtrS);
            Object objD;
            objD.emplace(tid, createAsPtrD);
            return Component(tid, objS, objD, createAsValS, createAsValD, fields, init);
        }

        template <typename T, typename Imp, typename Tuple, std::size_t... Is>
        static Component makeImpl(UsageFunc createAsPtrS, UsageFunc createAsValS, UsageFunc createAsPtrD, UsageFunc createAsValD, std::index_sequence<Is...>, //
                                  FieldsFunc fields,
                                  InitFunc init)
        {
            Object objS; // map from type id => function to get/crate the required type of value.
            if (createAsPtrS)
            {
                ((registerInterface<T, Imp, Tuple, Is>(objS, createAsPtrS)), ...);
            }
            Object objD; // map from type id => function to get/crate the required type of value.
            if (createAsPtrD)
            {
                ((registerInterface<T, Imp, Tuple, Is>(objD, createAsPtrD)), ...);
            }

            return Component(typeid(T), objS, objD, createAsValS, createAsValD, fields, init);
        }

        template <typename T, typename Imp, typename Tuple, std::size_t I>
        static void registerInterface(Object &obj, UsageFunc createAsPtr)
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

            UsageFunc pFunc;
            UsageFunc vFunc;
            Context(UsageFunc pF, UsageFunc vF) : pFunc(pF), vFunc(vF)
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
            T *getPtr(std::type_index tid, Usage usgR)
            {
                return getComponent(tid).getPtr<T>(usgR);
            }

            template <typename T, typename Usage usg>
            UsageFunc getCtxFunc()
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
                UsageFunc func = usg & AsPtr ? it->second.top().pFunc : it->second.top().vFunc;
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
                UsageFunc vf;
                bindComp(Component::make(typeid(T), ptrFunc, vf));
            }

            template <typename T, typename F>
            void bindFuncAsVal(F &&valueFuncS)
            {
                UsageFunc vf;
                bindComp(Component::make(typeid(T), vf, valueFuncS));
            }

            template <typename T, typename C, typename F>
            void bindArgOfConstructorAsVal(F &&func)
            {

                bindFuncAsVal<ArgOfConstructor<T, C>>([func]() -> ArgOfConstructor<T, C>
                                                      { return ArgOfConstructor<T, C>(nullptr, func); });
            }

            template <Usage usg, typename T>
            void bindImpl()
            {
                bindComp(makeByImpl<usg, T, T>());
            }

            template <typename T>
            void bindImplAsPtrStatic()
            {
                bindComp(makeByImpl<AsPtrStatic, T, T>());
            }

            //
            template <typename T, typename Imp>
            void bindImplAsPtrStatic()
            {
                bindComp(makeByImpl<AsPtrStatic, T, Imp>());
            }

            template <typename T, typename Imp, typename T1>
            void bindImplAsPtrStatic()
            {
                bindComp(makeByImpl<AsPtrStatic, T, Imp, T1>());
            }

            template <typename T, typename Imp>
            void bindImplAsValStatic()
            {
                bindComp(makeByImpl<AsValStatic, T, Imp>());
            }

            template <typename T, typename Imp, typename F>
            void bindImplAsValStatic(F &&fieldsF)
            {

                bindComp(makeByImpl<AsValStatic, T, Imp>(fieldsF));
            }

            template <typename T, typename F>
            void bindImplAsValStatic(F &&fieldsF)
            {

                bindComp(makeByImpl<AsValStatic, T, T>(fieldsF));
            }

            template <typename T>
            void bindImplAsValStatic()
            {
                bindComp(makeByImpl<AsValStatic, T, T>());
            }

            template <typename... T>
            void bindAllImplAsPtrStatic()
            {
                ((bindImplAsPtrStatic<T>()), ...);
            }

            template <typename... T>
            void bindAllImplAsValStatic()
            {
                ((bindImplAsValStatic<T>()), ...);
            }

            template <typename F>
            void bindMissingTypesForFields(F &&func)
            {
                //
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

                UsageFunc ptrFunc = [this]()
                {
                    return getCtxFunc<T, AsPtr>()();
                };

                UsageFunc valFunc = [this]()
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

            template <typename T, typename Imp, typename T1, typename T2, Usage usg = AsPtrStatic>
            void bindImplAsPtrStatic()
            {
                bindComp(makeByImpl<usg, T, Imp, T1, T2>());
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

            template <typename T, typename OT>
            void bindMethodAsVal(T (OT::*method)())
            {
                bindFuncAsVal<T>([this, method]() -> T
                                 {
                                     OT *obj = this->getPtr<OT>();
                                     return (obj->*method)(); //
                                 });
            }

            template <typename T, typename OT>
            void bindMemberAsPtr(T *OT::*member)
            {
                bindFuncAsPtr<T>([this, member]() -> T *
                                 {
                                     OT *obj = this->getPtr<OT>();
                                     return obj->*member; //
                                 });
            }
            //

            template <Usage usg, typename T, typename Imp, typename... Adts>
            Component makeByImpl()
            {
                std::function<bool(std::type_index, std::string, std::any &)> fields;
                return makeByImpl<usg, T, Imp, Adts...>(fields);
            }
            template <Usage usg, typename T, typename Imp, typename... Adts>
            Component makeByImpl(std::function<bool(std::type_index, std::string, std::any &)> fields)
            {
                using TAdtsTuple = std::tuple<T, Adts...>;
                UsageFunc funcAsPtrStatic;  // empty func default.
                UsageFunc funcAsValStatic;  // empty func default.
                UsageFunc funcAsPtrDynamic; // empty func default.
                UsageFunc funcAsValDynamic; // empty func default.

                makeFunctionForUsage<usg, T, Imp>(funcAsPtrStatic, funcAsValStatic, funcAsPtrDynamic, funcAsValDynamic); //
                InitFunc initFunc;
                makeFunctionForInit<T>(initFunc);

                return Component::makeImpl<T, Imp, TAdtsTuple>(funcAsPtrStatic, funcAsValStatic, funcAsPtrDynamic, funcAsValDynamic,    //
                                                               std::make_index_sequence<std::tuple_size_v<std::decay_t<TAdtsTuple>>>{}, //
                                                               fields,
                                                               initFunc);
            }
            template <typename T>
            std::enable_if_t<!hasVoidInit<T>::value, void> makeFunctionForInit(std::function<bool(std::any)> &initFunc)
            {
            }

            template <typename T>
            std::enable_if_t<hasVoidInit<T>::value, void> makeFunctionForInit(std::function<bool(std::any)> &initFunc)
            {
                initFunc = [](std::any ptrA)
                {
                    T *ptr = std::any_cast<T *>(ptrA);
                    ptr->init();
                    return true;
                };
            }

            template <Usage usg>
            void usageFault()
            {
                throw std::runtime_error("usage does not supported for the component registered.");
            }

            template <Usage usg, typename T, typename Imp>
            typename std::enable_if_t<usg & AsPtr && !(usg & AsVal), void> makeFunctionForUsage(UsageFunc &funcAsPtrS, UsageFunc &funcAsValS, UsageFunc &funcAsPtrD, UsageFunc &funcAsValD)
            {
                if (usg & AsStatic)
                {

                    funcAsPtrS = [this]() -> std::any
                    {
                        return std::make_any<T *>(this->getPtrStatic<Imp>());
                    };
                }
                if (usg & AsDynamic)
                {

                    funcAsPtrD = [this]() -> std::any
                    {
                        return std::make_any<T *>(this->getPtrDynamic<Imp>());
                    };
                }
            }

            template <Usage usg, typename T, typename Imp>
            typename std::enable_if_t<usg & AsVal && !(usg & AsPtr), void> makeFunctionForUsage(UsageFunc &funcAsPtrS, UsageFunc &funcAsValS, UsageFunc &funcAsPtrD, UsageFunc &funcAsValD)
            {
                if (usg & AsStatic)
                {

                    funcAsValS = [this]() -> std::any
                    {
                        return std::make_any<T>(this->getValStatic<Imp>());
                    };
                }
                if (usg & AsDynamic)
                {

                    funcAsValD = [this]() -> std::any
                    {
                        return std::make_any<T>(this->getValDynamic<Imp>());
                    };
                }
            }

            template <Usage usg, typename T, typename Imp>
            typename std::enable_if_t<usg & AsPtr && usg & AsVal, void> makeFunctionForUsage(UsageFunc &funcAsPtrS, UsageFunc &funcAsValS, UsageFunc &funcAsPtrD, UsageFunc &funcAsValD)
            {
                if (usg & AsStatic)
                {

                    funcAsPtrS = [this]() -> std::any
                    {
                        return std::make_any<T *>(this->getPtrStatic<Imp>());
                    };
                }
                if (usg & AsDynamic)
                {

                    funcAsPtrD = [this]() -> std::any
                    {
                        return std::make_any<T *>(this->getPtrDynamic<Imp>());
                    };
                }

                if (usg & AsStatic)
                {

                    funcAsValS = [this]() -> std::any
                    {
                        return std::make_any<T>(this->getValStatic<Imp>());
                    };
                }
                if (usg & AsDynamic)
                {

                    funcAsValD = [this]() -> std::any
                    {
                        return std::make_any<T>(this->getValDynamic<Imp>());
                    };
                }
            }

            template <typename T>
            T *getPtrStatic()
            {
                static T *instance = createInstance<AsPtr, T>();
                return (instance);
            }

            template <typename T>
            T *getPtrDynamic()
            {

                T *ptr = createInstance<AsPtr, T>();
                return ptr;
            }

            template <typename T>
            T getValStatic()
            {
                static T instance = createInstance<AsVal, T>();

                return (instance);
            }

            template <typename T>
            T getValDynamic()
            {
                T ret = createInstance<AsVal, T>();
                return ret;
            }

            //
            template <typename T>
            T *getPtr(Usage usgR = AsStatic)
            {
                return getComponent(typeid(T)).getPtr<T>(usgR);
            }

            template <typename T>
            bool tryGetVal(T &val, Usage usgR = AsStatic)
            {
                Component *cPtr = tryGetComponent(typeid(T));
                if (cPtr)
                {
                    val = cPtr->getVal<T>();
                    return true;
                }
                return false;
            }

            template <typename T>
            void push(std::function<T *()> ptrCtx, std::function<T()> valCtx)
            {
                if (auto it = this->contexts.find(typeid(T)); it != this->contexts.end())
                {
                    UsageFunc pFunc;
                    if (ptrCtx)
                    {
                        pFunc = [ptrCtx]()
                        { return std::make_any<T *>(ptrCtx()); };
                    }

                    UsageFunc vFunc;
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
            T getVal(Usage usgR = AsStaticFirst)
            {
                return getComponent(typeid(T)).getVal<T>(usgR);
            }
            template <typename T>
            T getVal(std::type_index tid, Usage usgR)
            {
                return getComponent(tid).getVal<T>(usgR);
            }

            Component &getComponent(std::type_index tid)
            {
                Component *cPtr = this->tryGetComponent(tid);
                if (!cPtr)
                {
                    throw std::runtime_error("must bind before get the instance by type from Component::Injector.");
                }
                return *cPtr;
            }

            template <typename T>
            Component *tryGetComponent()
            {
                return tryGetComponent(typeid(T));
            }

            Component *tryGetComponent(std::type_index tid)
            {
                if (auto it = components.find(tid); it != components.end())
                {
                    return &it->second;
                }
                return nullptr;
            }

            template <typename T>
            bool hasBind()
            {
                auto it = factories.find(typeid(T));
                return it != factories.end();
            }

            // void bindDefault(Injector &pInjector)
            // {
            //     this->defaultCompFunc = [&pInjector](std::type_index tid)
            //     { return pInjector.getComponent(tid); };
            // }

            Injector &operator=(const Injector &injector)
            {
                this->components = injector.components;
                // this->defaultCompFunc = injector.defaultCompFunc;
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
                // use default constructor
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
                // usgR is the runtime arg provided by the top most getPtr(usgR), this argument control only the outer most object creation.
                // do dynamic usge, do not propagate to deep layer, may be useful for other usage after unset the AsDynamic mask.
                //
                T *ret = new T(getAsConstructorArg<T, Is, std::tuple_element_t<Is, ArgsTuple>>()...);
                init<T>(ret);
                return ret;
            }
            // C<2>:As Value
            template <Usage usg, typename T, typename ArgsTuple, std::size_t... Is>
            typename std::enable_if_t<usg & AsVal, T>
            createInstanceByConstructor(std::index_sequence<Is...>)
            {

                // static_assert(allArgsArePointers<ArgsTuple>, "All inject arguments must be pointer types!");

                T ret = T(getAsConstructorArg<T, Is, std::tuple_element_t<Is, ArgsTuple>>()...);
                init<T>(&ret);
                return ret;
            }

            template <typename T>
            void init(T *ptr)
            {

                AutoRegisteredObjects &autoRegObjs = AutoRegisteredObjects::getInstance();
                if (auto itObj = autoRegObjs.objects.find(std::type_index(typeid(T))); itObj != autoRegObjs.objects.end()) // make init func to setting fields.
                {
                    std::any objPA = std::make_any<T *>(ptr);
                    const AutoRegisteredObjects::ObjectInfo &objInfo = itObj->second;
                    setRegistedFields<T>(objPA, objInfo);
                    callRegistedInit<T>(objPA, objInfo);
                }
            }

            template <typename T>
            void callRegistedInit(std::any &objPA, const AutoRegisteredObjects::ObjectInfo &objInfo)
            {
                for (const auto &init : objInfo.inits)
                {
                    init.method(objPA);
                }
            }
            template <typename T>
            void setRegistedFields(std::any &objPA, const AutoRegisteredObjects::ObjectInfo &objInfo)
            {
                //

                for (const auto &fieldPair : objInfo.fields)
                {
                    const std::string &fieldName = fieldPair.first;
                    const AutoRegisteredObjects::FieldInfo &fieldInfo = fieldPair.second;
                    //
                    // std::any fv = std::make_any<float>(1.0f);

                    std::any fv;
                    Component *cPtr = this->tryGetComponent(fieldInfo.vType);
                    if (cPtr)
                    {
                        if (fieldInfo.asPtr)
                        {
                            fv = cPtr->getPtr(AsStaticFirst, fieldInfo.vType);
                        }
                        else
                        {
                            fv = cPtr->getVal(AsStaticFirst, fieldInfo.vType);
                        }
                        //
                    }
                    else
                    { // no component bind to the type, try to get registed fields.
                        Component *pCPtr = this->tryGetComponent(typeid(T));
                        if (pCPtr->fields)
                        {
                            if (!pCPtr->fields(fieldInfo.vType, fieldName, fv))
                            {
                                throw std::runtime_error("failed to resolve the field value by field function.");
                            }
                        }
                        else
                        {
                            throw std::runtime_error("cannot resolve the field value because no field function registered.");
                        }
                    }

                    fieldInfo.setter(objPA, fv);

                } // fields
            }

            // G<1>
            template <typename C, std::size_t I, typename Arg>
            typename std::enable_if_t<std::is_pointer_v<Arg>, Arg> getAsConstructorArg() // return Arg or Arg *
            {
                Component *cPtr = this->tryGetComponent<std::remove_pointer_t<Arg>>();
                if (cPtr)
                {
                    return cPtr->getPtr<std::remove_pointer_t<Arg>>(AsStaticFirst);
                }

                cPtr = this->tryGetComponent<ArgOfConstructor<std::remove_pointer_t<Arg>, C>>();
                if (cPtr)
                {
                    auto cArg = cPtr->getVal<ArgOfConstructor<std::remove_pointer_t<Arg>, C>>(AsStaticFirst);
                    std::any ptrA = (cArg.ptrFunc)();
                    return std::any_cast<Arg>(ptrA);
                }
                throw std::runtime_error("cannot resolve component for a arg of constructor.");
            }

            // G<2>
            template <typename C, std::size_t I, typename Arg>
            typename std::enable_if_t<!std::is_pointer_v<Arg>, Arg> getAsConstructorArg() // return Arg or Arg *
            {

                Component *cPtr = this->tryGetComponent<Arg>();
                if (cPtr)
                {
                    return cPtr->getVal<Arg>(AsStaticFirst);
                }

                cPtr = this->tryGetComponent<ArgOfConstructor<Arg, C>>();
                if (cPtr)
                {
                    auto cArg = cPtr->getVal<ArgOfConstructor<Arg, C>>(AsStaticFirst);
                    std::any valA = (cArg.valFunc)();
                    return std::any_cast<Arg>(valA);
                }
                throw std::runtime_error("cannot resolve component for a arg of constructor.");
            }
        };
        //
    };
};
