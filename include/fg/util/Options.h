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

    class Options
    {
    public:
        struct Option
        {
            std::string name;
            std::any valuePtr;
            std::function<void()> destructor;

            ~Option()
            {
                destructor();
            }
            template <typename T>
            Option(std::string name, T defaultV) : name(name)
            {
                T *valueP = new T(defaultV);
                this->valuePtr = std::make_any<T *>(valueP);
                this->destructor = [valueP]()
                {
                    delete valueP;
                };
            }

            Option(const Option &) = delete;            // copy
            Option(Option &&) = delete;                 // move
            Option &operator=(const Option &) = delete; // copy assign
            Option &operator=(Option &&) = delete;      // move assign

            template <typename T>
            bool isType() const
            {
                return valuePtr.type() == typeid(T *);
            }

            template <typename T>
            T &getValueRef() const
            {
                T *p = std::any_cast<T *>(valuePtr);
                return *p;
            }
        };

    public:
        template <typename T>
        static T get(Options &opts, std::string name, T defValue)
        {
            Option *opt = opts.getOption(name);
            if (!opt)
            {
                return defValue;
            }
            if (opt->isType<T>())
            {
                return opt->getValueRef<T>();
            }
            throw std::runtime_error(fmt::format("type of option:{} is mismatch.", name));
        }

    protected:
        std::unordered_map<std::string, std::unique_ptr<Option>> options;

    public:
    public:
        ~Options()
        {
        }
        Option *getOption(std::string name)
        {
            auto &it = options.find(name);
            if (it == options.end())
            {
                return nullptr;
            }
            return it->second.get();
        }

        template <typename T>
        Option *add(std::string name, T defaultValue)
        {
            Option *ret = tryAdd(name, defaultValue);

            if (ret)
            {
                return ret;
            }
            throw new std::runtime_error("cannot create options with name:" + name);
        }

        template <typename T>
        Option *tryAdd(std::string name, T defaultValue)
        {
            Option *opt = nullptr;
            auto it = options.find(name);
            if (it == options.end())
            {
                std::unique_ptr<Option> optionPtr = std::make_unique<Option>(name, defaultValue);
                //<std::unique_ptr<Option>>
                opt = optionPtr.get();
                options[name] = std::move(optionPtr);
            }

            return opt;
        }

        template <typename F>
        void forEach(F &&visit)
        {
            for (const auto &pair : options)
            {
                visit(pair.first, pair.second.get());
            }
        }
    };

};
