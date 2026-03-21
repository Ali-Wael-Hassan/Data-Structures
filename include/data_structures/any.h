#pragma once
#include "type.h"

namespace algolib {

class any {
private:
    void* data = nullptr;
    type_info type;
    void (*deleter)(void*) = nullptr;

public:
    // default constructor = empty
    any() noexcept : data(nullptr), type(), deleter(nullptr) {}

    // constructor to initialize with a value
    template<typename T, typename U = decay_t<T>>
    any(T&& value) {
        set<U>(static_cast<T&&>(value));
    }

    // destructor
    ~any() { reset(); }

    // move constructor
    any(any&& other) noexcept
        : data(other.data), type(other.type), deleter(other.deleter) 
    {
        other.data = nullptr;
        other.type = type_info();
        other.deleter = nullptr;
    }

    // move assignment
    any& operator=(any&& other) noexcept {
        if (this != &other) {
            reset();
            data = other.data;
            type = other.type;
            deleter = other.deleter;

            other.data = nullptr;
            other.type = type_info();
            other.deleter = nullptr;
        }
        return *this;
    }

    // disable copy
    any(const any&) = delete;
    any& operator=(const any&) = delete;

    // set / emplace a value
    template<typename T, typename U = decay_t<T>>
    void set(T&& value) {
        reset();
        U* ptr = new U(static_cast<T&&>(value));
        data = ptr;
        type = type_id<U>();
        deleter = [](void* p){ delete static_cast<U*>(p); };
    }

    // reset to empty
    void reset() noexcept {
        if (deleter && data) deleter(data);
        data = nullptr;
        deleter = nullptr;
        type = type_info();
    }

    // check type
    template<typename T>
    bool is() const noexcept {
        return type == type_id<T>();
    }

    // get pointer to value, or nullptr if wrong type
    template<typename T>
    T* try_get() noexcept {
        return is<T>() ? static_cast<T*>(data) : nullptr;
    }

    template<typename T>
    const T* try_get() const noexcept {
        return is<T>() ? static_cast<const T*>(data) : nullptr;
    }

    type_info get_type() const noexcept {
        return type;
    }

    // check if any contains something
    bool has_value() const noexcept { return type.valid(); }
};

} // namespace algolib