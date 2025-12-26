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

    // 主模板
    template <typename Tuple>
    struct reverse_tuple;

    // 特化：匹配 std::tuple
    template <typename... Ts>
    struct reverse_tuple<std::tuple<Ts...>>
    {
    private:
        template <std::size_t... Is>
        static auto impl(std::index_sequence<Is...>)
            -> std::tuple<std::tuple_element_t<sizeof...(Ts) - 1 - Is, std::tuple<Ts...>>...>;

    public:
        using type = decltype(impl(std::make_index_sequence<sizeof...(Ts)>{}));
    };

    // 便捷别名（C++14+）
    template <typename Tuple>
    using reverse_tuple_t = typename reverse_tuple<Tuple>::type;

} // namespace fog