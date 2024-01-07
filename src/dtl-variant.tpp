#pragma once

namespace dtl {

template <typename _TIn>
struct variant_ptr;

template <typename... _TVal>
struct variant_ptr<std::variant<_TVal...>> {
    std::variant<_TVal*...> __value;

    using base = typename std::variant<_TVal...>;

    operator bool() const {
        return std::visit(
            [](auto&& val) -> bool {
                return val != NULL;
            },
            __value);
    }

    bool
    operator==(const variant_ptr& rhs) const = default;
};

template <typename... _TVal>
struct variant_ptr<const std::variant<_TVal...>> {
    std::variant<const _TVal*...> __value;

    using base = typename std::variant<_TVal...>;

    operator bool() const {
        return std::visit(
            [](auto&& val) -> bool {
                return val != NULL;
            },
            __value);
    }

    bool
    operator==(const variant_ptr& rhs) const = default;
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

template <typename _Tin>
struct shared_variant_ptr;

template <typename... _TVal>
struct shared_variant_ptr<std::variant<_TVal...>> {
    using _TStorage = std::variant<std::shared_ptr<_TVal>...>;
    _TStorage __value;

    using base = std::variant<_TVal...>;

    shared_variant_ptr() = default;
    shared_variant_ptr(const shared_variant_ptr& __rhs) = default;
    shared_variant_ptr(shared_variant_ptr&&) = default;
    shared_variant_ptr&
    operator=(const shared_variant_ptr&) = default;
    shared_variant_ptr&
    operator=(shared_variant_ptr&&) = default;

    template <
        typename _T,
        typename = std::enable_if_t<std::is_constructible_v<_TStorage, _T>>>
    shared_variant_ptr(const _T& value) :
        __value(value) {}

    template <
        typename _T,
        typename = std::enable_if_t<std::is_constructible_v<_TStorage, _T>>>
    shared_variant_ptr(_T&& value) :
        __value(std::move(value)) {}

    bool
    operator==(const shared_variant_ptr& rhs) const = default;

    operator bool() const { return __value; }
};

template <typename... _TVal>
struct shared_variant_ptr<const std::variant<_TVal...>> {
    using _TStorage = std::variant<std::shared_ptr<const _TVal>...>;
    _TStorage __value;

    using base = const std::variant<_TVal...>;

    shared_variant_ptr() = default;
    shared_variant_ptr(const shared_variant_ptr& __rhs) = default;
    shared_variant_ptr(shared_variant_ptr&&) = default;
    shared_variant_ptr&
    operator=(const shared_variant_ptr&) = default;
    shared_variant_ptr&
    operator=(shared_variant_ptr&&) = default;

    template <
        typename _T,
        typename = std::enable_if_t<std::is_constructible_v<_TStorage, _T>>>
    shared_variant_ptr(const _T& value) :
        __value(value) {}

    template <
        typename _T,
        typename = std::enable_if_t<std::is_constructible_v<_TStorage, _T>>>
    shared_variant_ptr(_T&& value) :
        __value(std::move(value)) {}

    bool
    operator==(const shared_variant_ptr& rhs) const = default;
};

template <typename _TPtr>
variant_ptr<typename _TPtr::base>
borrow(_TPtr& ptr) {
    variant_ptr<typename _TPtr::base> result;
    std::visit([&](auto&& ptr) { result.__value = &*ptr; }, ptr.__value);
    return result;
}

template <typename _TVariant>
shared_variant_ptr<_TVariant>
acquire(variant_ptr<_TVariant>& ptr) {
    return std::visit(
        [](auto&& val) -> shared_variant_ptr<_TVariant> {
            return shared_variant_ptr<_TVariant>(val->shared_from_this());
        },
        ptr.__value);
}

/**
 * `dtl::get_if` returns a pointer of the value inside a variant if it is of
 * the requested type or NULL if it is not.
 */
template <typename _T>
struct __get_if_impl {
    template <typename _V>
    _T
    operator()(_V variant) {
        _T* value = std::get_if<_T>(&variant.__value);
        return value == NULL ? NULL : *value;
    }
};

template <typename _TBase>
struct __get_if_impl<dtl::variant_ptr<_TBase>> {
    template <typename _Var>
    dtl::variant_ptr<_TBase>
    operator()(_Var variant) {
        return std::visit(
            [](auto&& val) {
                using _TVar = dtl::variant_ptr<_TBase>;
                using _TVal = std::decay_t<decltype(val)>;

                if constexpr (std::is_constructible_v<_TVar, _TVal>) {
                    return _TVar(val);
                }

                return _TVar();
            },
            variant.__value);
    }
};

template <typename _T, typename _V>
_T
get_if(_V variant) {
    return __get_if_impl<_T>()(variant);
}

/**
 * `dtl::merge` creates a new variant containing the union of the sets of types
 * contained in its arguments.
 */
template <typename... _TVariants>
struct __merge_impl;

template <typename... _TAVals, typename... _TBVals, typename... _TRest>
struct __merge_impl<std::variant<_TAVals...>, std::variant<_TBVals...>, _TRest...> {
    using type = typename __merge_impl<std::variant<_TAVals..., _TBVals...>, _TRest...>::type;
};

template <typename _TVariant>
struct __merge_impl<_TVariant> {
    using type = _TVariant;
};

template <typename... _TVariants>
using merge = typename __merge_impl<_TVariants...>::type;

/**
 * `dtl::cast` converts an instance of one variant type to an instance of a
 * different variant type that contains all of the alternatives in the first
 * variant type.
 */
template <typename _TResult>
struct __cast_impl;

template <typename... _TResultVals>
struct __cast_impl<dtl::variant_ptr<_TResultVals...>> {
    using _TResult = dtl::variant_ptr<_TResultVals...>;

    template <typename... _TSourceVals>
    _TResult
    operator()(dtl::variant_ptr<_TSourceVals...> input) const {
        return std::visit(
            [&](auto&& value) -> _TResult {
                return _TResult(value);
            },
            input.__value);
    }
};

template <typename... _TResultVals>
struct __cast_impl<dtl::shared_variant_ptr<_TResultVals...>> {
    using _TResult = dtl::shared_variant_ptr<_TResultVals...>;

    template <typename... _TSourceVals>
    _TResult
    operator()(dtl::shared_variant_ptr<_TSourceVals...> input) const {
        return std::visit(
            [&](auto&& value) -> _TResult {
                return _TResult(value);
            },
            input.__value);
    }
};

template <typename _TResult, typename _TSource>
_TResult
cast(_TSource input) {
    return __cast_impl<_TResult>()(input);
}

} /* namespace dtl */

template <typename _TVariant>
struct std::hash<dtl::variant_ptr<_TVariant>> {
    std::size_t
    operator()(const dtl::variant_ptr<_TVariant>& ptr) const {
        return std::visit(
            [](auto&& val) -> std::size_t {
                return std::hash<size_t>()(size_t(&*val));
            },
            ptr.__value);
    }
};

template <typename _TVariant>
struct std::hash<dtl::shared_variant_ptr<_TVariant>> {
    std::size_t
    operator()(const dtl::shared_variant_ptr<_TVariant>& ptr) const {
        return std::visit(
            [](auto&& val) -> std::size_t {
                return std::hash<size_t>()(size_t(&*val));
            },
            ptr.__value);
    }
};
