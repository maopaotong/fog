// TemplateUtil.h
#pragma once
#include <tuple>
#include <utility>

namespace fog
{
    template <typename T>
    struct isTuple : std::false_type
    {
    };

    template <typename... Ts>
    struct isTuple<std::tuple<Ts...>> : std::true_type
    {
    };
    /*
     */
    template <typename Tuple>
    struct tupleTail;
    template <typename Head, typename... Tail>
    struct tupleTail<std::tuple<Head, Tail...>>
    {
        using type = std::tuple<Tail...>;
    };

    template <typename Tuple>
    struct tupleHead;
    template <typename Head, typename... Tail>
    struct tupleHead<std::tuple<Head, Tail...>>
    {
        using type = Head;
    };

} // namespace fog