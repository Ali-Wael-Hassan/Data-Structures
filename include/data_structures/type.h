#pragma once

namespace algolib {

// remove references
template<typename T> struct remove_ref      { using type = T; };
template<typename T> struct remove_ref<T&>  { using type = T; };
template<typename T> struct remove_ref<T&&> { using type = T; };

// remove const/volatile
template<typename T> struct remove_cv                       { using type = T; };
template<typename T> struct remove_cv<const T>              { using type = T; };
template<typename T> struct remove_cv<volatile T>           { using type = T; };
template<typename T> struct remove_cv<const volatile T>     { using type = T; };

// decay type = remove ref + remove cv
template<typename T>
using decay_t = typename remove_cv<typename remove_ref<T>::type>::type;


class type_info {
private:
    const void* id;

    // only type_id<T>() can create a valid type_info
    constexpr explicit type_info(const void* p) noexcept : id(p) {}

public:
    // default "no type"
    constexpr type_info() noexcept : id(nullptr) {}

    template<typename T>
    friend constexpr type_info type_id() noexcept;

    // comparisons
    constexpr bool operator==(const type_info& other) const noexcept { return id == other.id; }
    constexpr bool operator!=(const type_info& other) const noexcept { return id != other.id; }

    constexpr bool operator<(const type_info& other) const noexcept { return id < other.id; }
    constexpr bool operator<=(const type_info& other) const noexcept { return id <= other.id; }
    constexpr bool operator>(const type_info& other) const noexcept { return id > other.id; }
    constexpr bool operator>=(const type_info& other) const noexcept { return id >= other.id; }

    // helpers
    constexpr bool valid() const noexcept { return id != nullptr; }
    constexpr const void* raw() const noexcept { return id; }
};


template<typename T>
struct type_id_holder {
    static inline char unique;
};


template<typename T>
constexpr type_info type_id() noexcept {
    using U = decay_t<T>;
    return type_info(&type_id_holder<U>::unique);
}

}