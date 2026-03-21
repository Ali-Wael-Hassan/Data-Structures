#pragma once

#include <string>

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

// checking if array
template<typename T> struct is_array { static constexpr bool value = false; };
template<typename T> struct is_array<T[]> { static constexpr bool value = true; };
template<typename T, unsigned long long N> struct is_array<T[N]> { static constexpr bool value = true; };

template<typename T> struct remove_extent { using type = T; };
template<typename T> struct remove_extent<T[]> { using type = T; };
template<typename T, unsigned long long N> struct remove_extent<T[N]> { using type = T; };

template<bool B, typename T, typename F> struct conditional { using type = T; };
template<typename T, typename F> struct conditional<false, T, F> { using type = F; };

template<typename T> struct is_char_ptr { static constexpr bool value = false; };
template<> struct is_char_ptr<char*> { static constexpr bool value = true; };
template<> struct is_char_ptr<const char*> { static constexpr bool value = true; };

template<typename T>
struct decay_helper {
    using U = typename remove_ref<T>::type;
    
    using Decayed = typename conditional<
        is_array<U>::value,
        typename remove_extent<U>::type*,
        typename remove_cv<U>::type
    >::type;

    using type = typename conditional<
        is_char_ptr<Decayed>::value,
        std::string, 
        Decayed
    >::type;
};

template<typename T>
using decay_t = typename decay_helper<T>::type;

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