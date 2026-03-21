#pragma once

namespace algolib {

template<class T, class Storage>
class stack {
private:
    Storage storage;

public:
    void push(const T& value) { storage.push_back(value); }
    void push(T&& value) { storage.push_back(std::move(value)); }
    
    void pop() { storage.pop_back(); }

    T& top() { return storage.back(); }
    const T& top() const { return storage.back(); }

    bool empty() const { return storage.empty(); }
    auto size() const { return storage.size(); }
    
    void clear() { storage.clear(); }
};

} // namespace algolib