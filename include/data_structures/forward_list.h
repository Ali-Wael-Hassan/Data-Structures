#pragma once

#include <memory>
#include <initializer_list>
#include <utility>
#include <algorithm>

namespace algolib {

    template<typename T>
    struct Node {
        T data;
        Node* next;
    };

    template <class T, class Allocator = std::allocator<Node<T>>>
    class forward_list {
    public:
        using reference       = T&;
        using const_reference = const T&;

        // ITERATOR
        class iterator {
        public:
            using pointer           = T*;
            using reference         = T&;
            
            iterator(Node<T>* p = nullptr) : ptr(p) {}
            reference operator*() const { return ptr->data; }
            pointer operator->() const { return &(ptr->data); }
            iterator& operator++() { if (ptr) ptr = ptr->next; return *this; }
            iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
            bool operator==(const iterator& other) const { return ptr == other.ptr; }
            bool operator!=(const iterator& other) const { return ptr != other.ptr; }
        private:
            Node<T>* ptr;
            friend class forward_list;
        };

        // CONSTRUCTOR
        forward_list() : m_head(nullptr) {}
        
        explicit forward_list(const Allocator& alloc) : m_alloc(alloc), m_head(nullptr) {}

        forward_list(std::initializer_list<T> init, const Allocator& alloc = Allocator()) 
            : m_alloc(alloc), m_head(nullptr) {
            if (init.size() == 0) return;
            
            auto it = init.begin();
            m_head = create_node(*it, nullptr);
            Node<T>* curr = m_head;
            ++it;
            
            for (; it != init.end(); ++it) {
                curr->next = create_node(*it, nullptr);
                curr = curr->next;
            }
        }

        forward_list(const forward_list& other) : m_alloc(other.m_alloc), m_head(nullptr) {
            if (!other.m_head) return;
            m_head = create_node(other.m_head->data, nullptr);
            Node<T>* curr = m_head;
            for (Node<T>* o_curr = other.m_head->next; o_curr; o_curr = o_curr->next) {
                curr->next = create_node(o_curr->data, nullptr);
                curr = curr->next;
            }
        }

        forward_list(forward_list&& other) noexcept : m_alloc(std::move(other.m_alloc)), m_head(other.m_head) {
            other.m_head = nullptr;
        }

        ~forward_list() { clear(); }

        // ASSIGNMENT
        forward_list& operator=(const forward_list& other) {
            if (this != &other) { forward_list temp(other); swap(temp); }
            return *this;
        }

        void swap(forward_list& other) noexcept {
            std::swap(m_head, other.m_head);
            std::swap(m_alloc, other.m_alloc);
        }

        // ITERATORS
        iterator begin() noexcept { return iterator(m_head); }
        iterator end() noexcept { return iterator(nullptr); }

        // ELEMENT ACCESS
        reference front() { return m_head->data; }
        const_reference front() const { return m_head->data; }

        // CAPACITY
        bool empty() const noexcept { return m_head == nullptr; }

        // MODIFIERS
        void push_front(const T& val) { m_head = create_node(val, m_head); }
        
        void pop_front() {
            if (!m_head) return;
            Node<T>* old = m_head;
            m_head = m_head->next;
            destroy_node(old);
        }

        iterator insert_after(iterator pos, const T& val) {
            if (pos.ptr == nullptr) return end();
            Node<T>* newNode = create_node(val, pos.ptr->next);
            pos.ptr->next = newNode;
            return iterator(newNode);
        }

        iterator erase_after(iterator pos) {
            if (pos.ptr == nullptr || pos.ptr->next == nullptr) return end();
            Node<T>* toDelete = pos.ptr->next;
            pos.ptr->next = toDelete->next;
            destroy_node(toDelete);
            return iterator(pos.ptr->next);
        }

        void clear() noexcept {
            while (!empty()) pop_front();
        }

        // OPERATIONS
        void sort() {
            if (!m_head || !m_head->next) return;

            Node<T>* bins[64] = { nullptr }; 
            Node<T>* curr = m_head;
            Node<T>* next_node = nullptr;

            while (curr) {
                next_node = curr->next;
                curr->next = nullptr;

                int i = 0;
                while (i < 64 && bins[i] != nullptr) {
                    curr = merge_internal(bins[i], curr);
                    bins[i] = nullptr;
                    i++;
                }

                if (i < 64) bins[i] = curr;
                curr = next_node;
            }

            Node<T>* result = nullptr;
            for (int i = 0; i < 64; i++) {
                result = merge_internal(bins[i], result);
            }
            m_head = result;
        }

        void reverse() noexcept {
            Node<T>* prev = nullptr;
            Node<T>* curr = m_head;
            while (curr) {
                Node<T>* next = curr->next;
                curr->next = prev;
                prev = curr;
                curr = next;
            }
            m_head = prev;
        }

        // RELATIONAL OPERATORS
        bool operator==(const forward_list& other) const {
            Node<T>* a = m_head; Node<T>* b = other.m_head;
            while (a && b) {
                if (a->data != b->data) return false;
                a = a->next; b = b->next;
            }
            return a == nullptr && b == nullptr;
        }
        bool operator<(const forward_list& other) const {
            return std::lexicographical_compare(iterator(m_head), iterator(nullptr),
                                               iterator(other.m_head), iterator(nullptr));
        }
        bool operator!=(const forward_list& other) const { return !(*this == other); }
        bool operator>(const forward_list& other) const { return other < *this; }
        bool operator<=(const forward_list& other) const { return !(*this > other); }
        bool operator>=(const forward_list& other) const { return !(*this < other); }

    private:
        Node<T>* merge_internal(Node<T>* l1, Node<T>* l2) {
            if (!l1) return l2;
            if (!l2) return l1;

            Node<T> dummy{T(), nullptr};
            Node<T>* tail = &dummy;

            while (l1 && l2) {
                if (l1->data < l2->data) {
                    tail->next = l1;
                    l1 = l1->next;
                } else {
                    tail->next = l2;
                    l2 = l2->next;
                }
                tail = tail->next;
            }
            tail->next = (l1) ? l1 : l2;
            return dummy.next;
        }

        Node<T>* create_node(const T& val, Node<T>* next) {
            Node<T>* ptr = m_alloc.allocate(1);
            try { new (ptr) Node<T>{val, next}; }
            catch (...) { m_alloc.deallocate(ptr, 1); throw; }
            return ptr;
        }

        void destroy_node(Node<T>* ptr) {
            ptr->~Node<T>();
            m_alloc.deallocate(ptr, 1);
        }

        Allocator m_alloc;
        Node<T>* m_head;
    };

    template <class T, class Alloc>
    inline void swap(forward_list<T, Alloc>& lhs, forward_list<T, Alloc>& rhs) noexcept {
        lhs.swap(rhs);
    }
}