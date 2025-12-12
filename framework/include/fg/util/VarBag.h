/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma once
#include <string>

namespace fog
{
template <typename T>
class Var
{
public:
    struct Range
    {
        T min;
        T max;
        Range(T min, T max) : min(min), max(max) {}
    };
    class Listener
    {
    public:
        virtual void onVarChanged(const std::string name, T &ref) = 0;
    };
    struct Bind
    {
        T value;
        Range *scope = nullptr;
        std::unordered_set<Listener *> listeners;
        Bind(Bind &vp)
        {
            VarPtr(vp.ptr);
        }
        Bind(T value) : value(value)
        {
        }
        ~Bind()
        {
            if (this->scope)
            {
                delete this->scope;
            }
        }
        void addListener(Listener *l)
        {
            listeners.insert(l);
        }
        void removeListener(Listener *l)
        {
            listener.erase(l);
        }

        void notify(const std::string &name)
        {
            for (auto it : listeners)
            {
                it->onVarChanged(name, value);
            }
        }
    };

    class Bag
    {
    public:
    private:
    private:
        std::unordered_map<std::string, Bind *> varMap;

    public:
        void setVar(const std::string name, T value)
        {

            Bind *vbp = this->getBind(name);

            if (vbp)
            {
                vbp->value = value;
            }
            else
            {
                void (*func)(Bind *) = [](Bind *) {};

                vbp = this->createBind(name, value, func);
            }
            vbp->notify(name);
        }

        T *createBindVptr(const std::string name, T value, T min, T max)
        {
            void (*func)(Bind *, T, T) = [](Bind *bind, T min, T max)
            { bind->scope = new Range(min, max); };

            return createBindVptr<T, T>(name, value, func, min, max);
        }

        template <typename... Args>
        T *createBindVptr(const std::string name, T defVal, void (*initFunc)(Bind *, Args...), Args... args)
        {
            Bind *vbp = this->createBind<Args...>(name, defVal, initFunc, args...);
            return &vbp->value;
        }

        template <typename... Args>
        Bind *createBind(const std::string name, T defVal, void (*initFunc)(Bind *, Args...), Args... args)
        {
            Bind *vbp = this->getBind(name);
            if (vbp)
            {
                return vbp;
            }
            vbp = new Bind(defVal);
            this->varMap.emplace(name, vbp); //
            initFunc(vbp, args...);
            return vbp;
        }
        Bind *getBind(const std::string name)
        {
            std::unordered_map<std::string, Bind *>::iterator it = this->varMap.find(name);
            if (it == this->varMap.end())
            {
                return nullptr;
            }
            return it->second;
        }

        T *getVarPtr(std::string name)
        {
            Bind *vb = this->getBind(name);
            if (vb)
            {
                return &vb->value;
            }
            return nullptr;
        }

        bool getVarVal(std::string name, T &ret)
        {
            T *ptr = this->getVarPtr(name);
            if (ptr)
            {
                ret = *ptr;
                return true;
            }
            return false;
        }

        template <typename T2>
        T2 getVarVal(std::string name, T2 (*cvtFunc)(T from), T2 def)
        {
            T *ptr = this->getVarPtr(name);
            if (ptr)
            {
                return def;
            }
            return cvtFunc(*ptr);
        }

        T getVarVal(std::string name, T def)
        {
            T *ptr = this->getVarPtr(name);
            if (ptr)
            {
                return *ptr;
            }
            return def;
        }

        template <typename... Args>
        void forEachVarPtr(void (*func)(const std::string, T *, Range *scope, Args... args), Args... args)
        {
            for (auto pair : varMap)
            {
                Bind *bind = pair.second;
                func(pair.first, &bind->value, bind->scope, args...);
            }
        }

        template <typename... Args>
        void forEachVarPtr(void (*func)(const std::string, T *, Args... args), Args... args)
        {
            for (auto pair : varMap)
            {
                Bind *bind = pair.second;
                func(pair.first, &bind->value, args...);
            }
        }
    };
};
};
