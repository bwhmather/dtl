#pragma once

namespace dtl {

template <typename _TIn>
struct variant_ptr;

template <typename... _TVal>
struct variant_ptr<std::variant<_TVal...>> {
    std::variant<_TVal*...> __value;

    using base = typename std::variant<_TVal...>;
};

template <typename... _TVal>
struct variant_ptr<const std::variant<_TVal...>> {
    std::variant<const _TVal*...> __value;

    using base = typename std::variant<_TVal...>;
};

template <typename _TIn>
struct unique_variant_ptr;

template <typename... _TVal>
struct unique_variant_ptr<std::variant<_TVal...>> {
    std::variant<std::unique_ptr<_TVal>...> __value;

    using base = typename std::variant<_TVal...>;

    unique_variant_ptr() = default;
    unique_variant_ptr(const unique_variant_ptr& __rhs) = delete;
    unique_variant_ptr(unique_variant_ptr&&) = default;
    unique_variant_ptr&
    operator=(const unique_variant_ptr&) = delete;
    unique_variant_ptr&
    operator=(unique_variant_ptr&&) = default;

    template <typename T>
    unique_variant_ptr(T&& value) :
        __value(std::move(value)) {}
};

template <typename... _TVal>
struct unique_variant_ptr<const std::variant<_TVal...>> {
    std::variant<std::unique_ptr<const _TVal>...> __value;

    using base = const std::variant<_TVal...>;

    unique_variant_ptr() = default;
    unique_variant_ptr(const unique_variant_ptr& __rhs) = delete;
    unique_variant_ptr(unique_variant_ptr&&) = default;
    unique_variant_ptr&
    operator=(const unique_variant_ptr&) = delete;
    unique_variant_ptr&
    operator=(unique_variant_ptr&&) = default;

    template <typename T>
    unique_variant_ptr(T&& value) :
        __value(std::move(value)) {}
};

template <typename _TPtr>
variant_ptr<typename _TPtr::base>
borrow(_TPtr& ptr) {
    variant_ptr<typename _TPtr::base> result;
    std::visit([&](auto&& ptr) { result.__value = &*ptr; }, ptr.__value);
    return result;
}

template <typename _T, typename _V>
_T
get_if(_V variant) {
    _T* value = std::get_if<_T>(&variant.__value);
    return value == NULL ? NULL : *value;
}

} /* namespace dtl */
