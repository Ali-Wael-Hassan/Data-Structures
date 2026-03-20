#pragma once
#include <memory>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace algolib {

    template <class T, class Allocator = std::allocator<T>>
    class vector {
    public:
        // Types

        using size_type = std::size_t;
        using value_type = T;

        // CONSTRUCTORS

        vector() noexcept(noexcept(Allocator())) : vector(Allocator()) {}

        explicit vector(const Allocator& alloc)
            : m_alloc(alloc), m_start(nullptr), m_end(nullptr), m_storageEnd(nullptr) {}

        explicit vector(size_type count, const Allocator& alloc = Allocator())
            : m_alloc(alloc), m_start(nullptr), m_end(nullptr), m_storageEnd(nullptr) 
        {
            if (count > 0) {
                m_start = m_end = m_alloc.allocate(count);
                m_storageEnd = m_start + count;
                try {
                    while (m_end < m_storageEnd) {
                        new (m_end++) value_type();
                    }
                } catch (...) {
                    internal_destroy_and_free();
                    throw;
                }
            }
        }

        vector(size_type count, const T& value, const Allocator& alloc = Allocator())
            : m_alloc(alloc), m_start(nullptr), m_end(nullptr), m_storageEnd(nullptr)
        {
            if (count > 0) {
                m_start = m_end = m_alloc.allocate(count);
                m_storageEnd = m_start + count;
                try {
                    while (m_end < m_storageEnd) {
                        new (m_end++) value_type(value);
                    }
                } catch (...) {
                    internal_destroy_and_free();
                    throw;
                }
            }
        }

        vector(const vector& other)
            : m_alloc(other.m_alloc), m_start(nullptr), m_end(nullptr), m_storageEnd(nullptr)
        {
            size_type count = other.size();
            if (count > 0) {
                m_start = m_end = m_alloc.allocate(count);
                m_storageEnd = m_start + count;
                try {
                    for (T* it = other.m_start; it != other.m_end; ++it) {
                        new (m_end++) value_type(*it);
                    }
                } catch (...) {
                    internal_destroy_and_free();
                    throw;
                }
            }
        }

        constexpr vector(vector&& other) noexcept
            : m_alloc(std::move(other.m_alloc)),
              m_start(other.m_start),
              m_end(other.m_end),
              m_storageEnd(other.m_storageEnd)
        {
            other.m_start = other.m_end = other.m_storageEnd = nullptr;
        }

        vector(vector&& other, const Allocator& alloc)
            : m_alloc(alloc), m_start(nullptr), m_end(nullptr), m_storageEnd(nullptr) 
        {
            if (m_alloc == other.m_alloc) {
                m_start = other.m_start;
                m_end = other.m_end;
                m_storageEnd = other.m_storageEnd;
                other.m_start = other.m_end = other.m_storageEnd = nullptr;
            } else {
                size_type count = other.size();
                if (count > 0) {
                    m_start = m_end = m_alloc.allocate(count);
                    m_storageEnd = m_start + count;
                    try {
                        for (T* it = other.m_start; it != other.m_end; ++it) {
                            new (m_end++) value_type(std::move(*it));
                        }
                    } catch (...) {
                        internal_destroy_and_free();
                        throw;
                    }
                }
            }
        }

        vector(std::initializer_list<T> init, const Allocator& alloc = Allocator())
            : m_alloc(alloc), m_start(nullptr), m_end(nullptr), m_storageEnd(nullptr)
        {
            if (init.size() > 0) {
                m_start = m_end = m_alloc.allocate(init.size());
                m_storageEnd = m_start + init.size();
                try {
                    for (const auto& item : init) {
                        new (m_end++) value_type(item);
                    }
                } catch (...) {
                    internal_destroy_and_free();
                    throw;
                }
            }
        }

        // DESTRUCTOR

        ~vector() {
            internal_destroy_and_free();
        }

        // ELEMENT ACCESS

        constexpr value_type& operator[](size_type pos) noexcept { return m_start[pos]; }
        constexpr const value_type& operator[](size_type pos) const noexcept { return m_start[pos]; }

        T& at(size_type pos) {
            if(pos >= size()) throw std::out_of_range( "DUCK::vector::at out of range access" );
            return m_start[pos];
        }

        constexpr const T& at(size_type pos) const {
            if(pos >= size()) throw std::out_of_range( "DUCK::vector::at out of range access" );
            return m_start[pos];
        }

        T& front() { return *m_start; }
        T& back() { return *(m_end - 1); }

        // BASIC ITERATORS SIMULATION

        constexpr value_type* begin() noexcept { return m_start; }
        constexpr value_type* end() noexcept { return m_end; }
        constexpr const value_type* begin() const noexcept { return m_start; }
        constexpr const value_type* end() const noexcept { return m_end; }

        // PUBLIC API

        void clear() noexcept {
            while (m_end > m_start) {
                (--m_end)->~value_type();
            }
        }

        // ALLOCATION
        
        void reserve(size_type new_cap) {
            if(new_cap <= capacity()) [[likely]] return;

            value_type* new_start = m_alloc.allocate(new_cap);
            value_type* new_end = new_start;

            try {
                for (value_type* it = m_start; it != m_end; ++it) {
                    new (new_end++) value_type(std::move(*it));
                }
            } catch (...) {
                m_alloc.deallocate(new_start, new_cap);
                throw;
            }

            size_type old_size = size();
            internal_destroy_and_free();
            
            m_start = new_start;
            m_end = new_start + old_size;
            m_storageEnd = m_start + new_cap;
        }

        void assign(size_type count, const T& value) {
            clear();
            reserve(count);

            while (m_end < m_start + count) {
                new (m_end++) value_type(value);
            }
        }

        void assign(std::initializer_list<T> ilist) {
            clear();
            reserve(ilist.size());

            for (const auto& item : ilist) {
                new (m_end++) value_type(item);
            }
        }

        // MODIFIERS

        void push_back(const value_type& value) {
            if (m_end == m_storageEnd) {
                reserve(capacity() ? 2 * capacity() : 1);
            }
            
            new (m_end++) value_type(value);
        }

        void push_back(value_type&& value) {
            if (m_end == m_storageEnd) {
                reserve(capacity() ? 2 * capacity() : 1);
            }
            
            new (m_end++) value_type(std::move(value));
        }

        template<typename... Args>
        value_type& emplace_back(Args&&... args) {
            if (m_end == m_storageEnd) {
                reserve(capacity() ? 2 * capacity() : 1);
            }
            
            new (m_end) value_type(std::forward<Args>(args)...);
            return *m_end++;
        }

        void pop_back() {
            if (empty()) return;
            (--m_end)->~value_type();
        }

        void resize(size_type count) {
            if (count < size()) {
                while (size() > count) {
                    pop_back();
                }
            } else if (count > size()) {
                if (count > capacity()) {
                    reserve(count);
                }
                while (size() < count) {
                    new (m_end++) value_type();
                }
            }
        }

        void swap(vector& other) noexcept {
            std::swap(m_alloc, other.m_alloc);
            std::swap(m_start, other.m_start);
            std::swap(m_end, other.m_end);
            std::swap(m_storageEnd, other.m_storageEnd);
        }

        void shrink_to_fit() {
            if (m_end == m_storageEnd) return;

            if (empty()) {
                internal_destroy_and_free();
            } else {
                size_type current_size = size();
                value_type* new_start = m_alloc.allocate(current_size);
                value_type* new_ptr = new_start;

                try {
                    for (value_type* it = m_start; it != m_end; ++it) {
                        new (new_ptr++) value_type(std::move(*it));
                    }
                } catch (...) {
                    m_alloc.deallocate(new_start, current_size);
                    throw;
                }

                internal_destroy_and_free();
                m_start = new_start;
                m_end = m_storageEnd = new_start + current_size;
            }
        }

        value_type* insert(value_type* pos, const T& value) {
            size_type offset = pos - m_start;
            if (m_end == m_storageEnd) {
                reserve(capacity() ? 2 * capacity() : 1);
            }
            
            pos = m_start + offset;

            if (pos != m_end) {
                for (value_type* it = m_end; it > pos; --it) {
                    new (it) value_type(std::move(*(it - 1)));
                    (it - 1)->~value_type();
                }
            }
            
            new (pos) value_type(value);
            m_end++;
            return pos;
        }

        void erase(value_type* pos) {
            if (pos < m_start || pos >= m_end) return;

            for (value_type* it = pos; it + 1 < m_end; ++it) {
                *it = std::move(*(it + 1)); 
            }

            (--m_end)->~value_type();
        }

        // ASSIGNMENT

        vector& operator=(const vector& other) {
            if (this != &other) {
                vector temp(other);
                std::swap(m_start, temp.m_start);
                std::swap(m_end, temp.m_end);
                std::swap(m_storageEnd, temp.m_storageEnd);
            }
            return *this;
        }

        vector& operator=(vector&& other) noexcept {
            if (this != &other) {
                internal_destroy_and_free();
                m_alloc = std::move(other.m_alloc);
                m_start = other.m_start;
                m_end = other.m_end;
                m_storageEnd = other.m_storageEnd;
                other.m_start = other.m_end = other.m_storageEnd = nullptr;
            }
            return *this;
        }

        // LOGICAL OPERATORS

        constexpr bool operator==(const vector& other) const {
            if (size() != other.size()) {
                return false;
            }

            for (size_type i = 0; i < size(); ++i) {
                if (m_start[i] != other.m_start[i]) {
                    return false;
                }
            }

            return true;
        }

        constexpr bool operator!=(const vector& other) const {
            return !(*this == other);
        }

        constexpr bool operator<(const vector& other) const {
            T* first1 = m_start;
            T* last1 = m_end;
            T* first2 = other.m_start;
            T* last2 = other.m_end;

            for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
                if (*first1 < *first2) return true;
                if (*first2 < *first1) return false;
            }
            return (first1 == last1) && (first2 != last2);
        }

        constexpr bool operator<=(const vector& other) const {
            return !(other < *this);
        }

        constexpr bool operator>(const vector& other) const {
            return other < *this;
        }

        constexpr bool operator>=(const vector& other) const {
            return !(*this < other);
        }

        // GETTERS

        constexpr size_type size() const noexcept { return m_end - m_start; }
        constexpr size_type capacity() const noexcept { return m_storageEnd - m_start; }

        constexpr bool empty() const noexcept { return m_start == m_end; }

        constexpr value_type* data() noexcept { return m_start; }
        constexpr const value_type* data() const noexcept { return m_start; }

    private:
        void internal_destroy_and_free() {
            clear();
            if (m_start) {
                m_alloc.deallocate(m_start, m_storageEnd - m_start);
            }
            m_start = m_end = m_storageEnd = nullptr;
        }

        Allocator m_alloc;
        value_type* m_start;
        value_type* m_end;
        value_type* m_storageEnd;
    };

    template <class T, class Alloc>
    inline void swap(vector<T, Alloc>& lhs, vector<T, Alloc>& rhs) noexcept {
        lhs.swap(rhs);
    }
}