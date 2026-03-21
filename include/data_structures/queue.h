#pragma once

namespace algolib {

template<class T, class Storage>
class queue {
private:
    Storage storage;

public:
    void push(const T& value) { storage.push_back(value); }
    void push(T&& value) { storage.push_back(std::move(value)); }
    
    void pop() { storage.pop_front(); }

    T& front() { return storage.front(); }
    const T& front() const { return storage.front(); }

    T& back() { return storage.back(); }
    const T& back() const { return storage.back(); }

    bool empty() const { return storage.empty(); }
    auto size() const { return storage.size(); }
    void clear() { storage.clear(); }
};

} // namespace algolib