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

public:
    object() = default;

    // constructor from value
    template<typename T>
    object(T&& value) : data(std::forward<T>(value)) {
        using U = decay_t<T>;
        printer = [](std::ostream& os, const any& storage) {
            if (U* ptr = storage.try_get<U>())
                os << *ptr;
        };
        reader = [](std::istream& is, any& storage) {
            U temp;
            if (is >> temp)
                storage.set(std::move(temp));
        };
    }

    // assignment
    template<typename T>
    object& operator=(T&& value) {
        data.set(std::forward<T>(value));
        using U = decay_t<T>;
        printer = [](std::ostream& os, const any& storage) {
            if (U* ptr = storage.try_get<U>())
                os << *ptr;
        };
        reader = [](std::istream& is, any& storage) {
            U temp;
            if (is >> temp)
                storage.set(std::move(temp));
        };
        return *this;
    }

    // implicit getter
    template<typename T>
    T& operator()() {
        return as<T>();
    }

    // cast operator
    template<typename T>
    operator T() const {
        const T* ptr = data.try_get<T>();
        if (!ptr) throw std::bad_cast();
        return *ptr;
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