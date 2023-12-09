#pragma once


namespace dtl {

template <typename _TIn>
struct __unique_variant_ptr_impl;

template <typename... _TVal>
struct __unique_variant_ptr_impl<std::variant<_TVal*...>> {
    using type = typename std::variant<std::unique_ptr<_TVal>...>;
};

template <typename... _TVal>
struct __unique_variant_ptr_impl<std::variant<_TVal...>> {
    using type = typename std::variant<std::unique_ptr<_TVal>...>;
};

template <typename... _TVal>
struct __unique_variant_ptr_impl<const std::variant<_TVal...>> {
    using type = typename std::variant<std::unique_ptr<const _TVal>...>;
};

template <typename _TIn>
using unique_variant_ptr_t = __unique_variant_ptr_impl<_TIn>::type;

template <typename _TIn>
struct __variant_ptr_impl;

template <typename... _TVal>
struct __variant_ptr_impl<std::variant<std::unique_ptr<_TVal>...>> {
    using type = typename std::variant<_TVal*...>;
};

template <typename... _TVal>
struct __variant_ptr_impl<const std::variant<std::unique_ptr<_TVal>...>> {
    using type = typename std::variant<_TVal*...>;
};

template <typename... _TVal>
struct __variant_ptr_impl<std::variant<_TVal...>> {
    using type = typename std::variant<_TVal*...>;
};

template <typename... _TVal>
struct __variant_ptr_impl<const std::variant<_TVal...>> {
    using type = typename std::variant<const _TVal*...>;
};

template <typename _TIn>
using variant_ptr_t = __variant_ptr_impl<_TIn>::type;

template <typename _TPtr>
variant_ptr_t<_TPtr>
borrow(_TPtr& ptr) {
    variant_ptr_t<_TPtr> result;
    std::visit([&](auto&& ptr) { result = &*ptr; }, ptr);
    return result;
}

} /* namespace dtl */
