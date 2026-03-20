#pragma once
#include <any>
#include <iostream>
#include <string>
#include <typeinfo>
#include <functional>

namespace algolib {

class object {
private:
    std::any data;

public:
    object() = default;

    template<typename T>
    object(T&& value) : data(std::forward<T>(value)) {}

    template<typename T>
    object& operator=(T&& value) {
        data = std::forward<T>(value);
        return *this;
    }

    const std::type_info& type() const { return data.type(); }

    bool is_empty() const { return !data.has_value(); }

    template<typename T>
    T& as() {
        return std::any_cast<T&>(data);
    }

    template<typename T>
    const T& as() const {
        return std::any_cast<const T&>(data);
    }
};

using list = std::vector<object>;

} // namespace algolib