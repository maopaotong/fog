#pragma once
#include "Options.h"
namespace fog
{
    
    template <typename T, typename = void>
    struct hasGroup : std::false_type
    {
    };

    template <typename T>
    struct hasGroup<T, std::void_t<decltype(T::Group)>> : std::true_type
    {
    };
    template <typename T>
    struct ConfigMembers
    {

        bool operator()(std::type_index ftype, std::string fname, std::any &fval)
        {
            Options::Groups *gps = groups();
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

        ConfigMembers(std::function<Options::Groups *()> groups) : groups(groups)
        {
        }

    private:
        std::function<Options::Groups *()> groups;

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