#pragma once

namespace dtl {

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

template <typename _TIn>
struct unique_variant_ptr_t;

template <typename... _TVal>
struct unique_variant_ptr_t<std::variant<_TVal...>> {
    std::variant<std::unique_ptr<_TVal>...> __value;

    using base = typename std::variant<_TVal...>;

    unique_variant_ptr_t() = default;
    unique_variant_ptr_t(const unique_variant_ptr_t& __rhs) = delete;
    unique_variant_ptr_t(unique_variant_ptr_t&&) = default;
    unique_variant_ptr_t&
    operator=(const unique_variant_ptr_t&) = delete;
    unique_variant_ptr_t&
    operator=(unique_variant_ptr_t&&) = default;

    template <typename T>
    unique_variant_ptr_t(T&& value) :
        __value(std::move(value)) {}
};

template <typename... _TVal>
struct unique_variant_ptr_t<const std::variant<_TVal...>> {
    std::variant<std::unique_ptr<const _TVal>...> __value;

    using base = const std::variant<_TVal...>;

    unique_variant_ptr_t() = default;
    unique_variant_ptr_t(const unique_variant_ptr_t& __rhs) = delete;
    unique_variant_ptr_t(unique_variant_ptr_t&&) = default;
    unique_variant_ptr_t&
    operator=(const unique_variant_ptr_t&) = delete;
    unique_variant_ptr_t&
    operator=(unique_variant_ptr_t&&) = default;

    template <typename T>
    unique_variant_ptr_t(T&& value) :
        __value(std::move(value)) {}
};

template <typename _TPtr>
variant_ptr_t<typename _TPtr::base>
borrow(_TPtr& ptr) {
    variant_ptr_t<typename _TPtr::base> result;
    std::visit([&](auto&& ptr) { result = &*ptr; }, ptr.__value);
    return result;
}

template <typename _T, typename _V>
_T
get_if(_V variant) {
    _T* value = std::get_if<_T>(&variant);
    return value == NULL ? NULL : *value;
}

} /* namespace dtl */
