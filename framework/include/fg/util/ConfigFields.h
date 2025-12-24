#pragma once

#include "Component.h"
namespace fog
{

    template <typename T>
    struct ConfigFields
    {

        bool operator()(std::type_index ftype, std::string fname, std::any &fval)
        {
            Options::Groups *gps = injector.getPtr<Options::Groups>();
            if (!gps)
            {
                throw std::runtime_error("no options groups found from injector.");
            }

            std::string gname = resolveGroup<T>();
            if (auto it = gps->groups.find(gname); it != gps->groups.end())
            {
                Options &ops = it->second;

                Options::Option *opt = ops.getOption(fname);
                if (opt)
                {
                    fval = opt->getValue();
                    return true;
                }
                throw std::runtime_error("cannot resolve option [" + gname + "]" + fname + "(no option found)");
            }
            throw std::runtime_error("cannot resolve option [" + gname + "]" + fname + "(no group found)");
        }

        ConfigFields(Component::Injector &injector) : injector(injector)
        {
        }

    private:
        Component::Injector &injector;

        template <typename X>
        static std::enable_if_t<hasGroup<X>::value, std::string> resolveGroup()
        {
            return X::Group;
        }

        template <typename X>
        static std::enable_if_t<!hasGroup<X>::value, std::string> resolveGroup()
        {
            throw std::runtime_error("cannot resolve group without X::Group");
        }
    };
};