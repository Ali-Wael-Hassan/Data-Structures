#pragma once
#include "any.h"
#include "vector.h"
#include <iostream>
#include <string>

namespace algolib {

class object {
private:
    any data;

    void (*printer)(std::ostream&, const any&) = nullptr;
    void (*reader)(std::istream&, any&) = nullptr;

    struct low_priority {};
    struct high_priority : low_priority {};

    template<typename U>
    auto set_printer(high_priority) -> decltype(std::declval<std::ostream&>() << std::declval<const U&>(), void()) {
        printer = [](std::ostream& os, const any& storage) {
            if (const U* ptr = storage.try_get<U>()) os << *ptr;
        };
    }

    template<typename U>
    void set_printer(low_priority) {
        printer = [](std::ostream& os, const any&) { os << "[Unprintable]"; };
    }

    template<typename U>
    auto set_reader(high_priority) -> decltype(std::declval<std::istream&>() >> std::declval<U&>(), void()) {
        reader = [](std::istream& is, any& storage) {
            U temp;
            if (is >> temp) storage.set(std::move(temp));
        };
    }

    template<typename U>
    void set_reader(low_priority) {
        reader = nullptr;
    }

public:
    object() = default;

    // constructor from value
    template<typename T>
    object(T&& value) : data(std::forward<T>(value)) {
        using U = decay_t<T>;
        set_printer<U>(high_priority{});
        set_reader<U>(high_priority{});
    }

    // assignment
    template<typename T>
    object& operator=(T&& value) {
        data.set(std::forward<T>(value));
        using U = decay_t<T>;

        set_printer<U>(high_priority{});
        set_reader<U>(high_priority{});
        return *this;
    }

    // implicit getter
    template<typename T>
    operator T&() {
        return as<T>();
    }

    template<typename T>
    operator const T&() const {
        return as<const T&>();
    }

    // cast operator
    template<typename T>
    operator T() const {
        const T* ptr = data.try_get<T>();
        if (!ptr) throw std::bad_cast();
        return *ptr;
    }

    template<typename T>
    friend object operator+(const object& lhs, const T& rhs) {
        return object(lhs.as<T>() + rhs);
    }

    friend object operator+(const object& lhs, const char* rhs) {
        return object(lhs.as<std::string>() + std::string(rhs));
    }

    friend object operator+(const object& lhs, const std::string& rhs) {
        return object(lhs.as<std::string>() + rhs);
    }

    template<typename T>
    friend object operator+(const T& lhs, const object& rhs) {
        return object(lhs + rhs.as<T>());
    }

    friend object operator+(const char* lhs, const object& rhs) {
        return object(std::string(lhs) + rhs.as<std::string>());
    }

    friend object operator+(const std::string& lhs, const object& rhs) {
        return object(lhs + rhs.as<std::string>());
    }

    // get type_info
    type_info type() const noexcept { 
        return data.has_value() ? data.get_type() : type_info(); 
    }

    bool is_empty() const noexcept { return !data.has_value(); }

    // access
    template<typename T>
    T& as() {
        T* ptr = data.try_get<T>();
        if (!ptr) throw std::bad_cast();
        return *ptr;
    }

    template<typename T>
    const T& as() const {
        const T* ptr = data.try_get<T>();
        if (!ptr) throw std::bad_cast();
        return *ptr;
    }

    template<typename T>
    T get_or(T fallback) const {
        const T* ptr = data.try_get<T>();
        return ptr ? *ptr : fallback;
    }

    // output operator
    friend std::ostream& operator<<(std::ostream& os, const object& obj) {
        if (obj.is_empty()) return os << "[None]";
        if (obj.printer) obj.printer(os, obj.data);
        else os << "[Unprintable Type]";
        return os;
    }

    // input operator
    friend std::istream& operator>>(std::istream& is, object& obj) {
        if (obj.is_empty()) {
            std::string temp;
            if (is >> temp)
                obj = temp;
        } else if (obj.reader) {
            obj.reader(is, obj.data);
        }
        return is;
    }
};

using python_list = vector<object>;

} // namespace algolib