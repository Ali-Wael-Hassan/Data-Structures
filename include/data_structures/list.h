#pragma once

#include <memory>
#include <initializer_list>
#include <utility>
#include <algorithm>
#include <iterator>

namespace algolib {

    template<typename T>
    struct Node {
        Node* prev;
        T data;
        Node* next;
    };

    template <class T, class Allocator = std::allocator<Node<T>>>
    class list {
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
            iterator& operator--() { if (ptr) ptr = ptr->prev; return *this; }
            
            iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
            iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }
            
            bool operator==(const iterator& other) const { return ptr == other.ptr; }
            bool operator!=(const iterator& other) const { return ptr != other.ptr; }
        private:
            Node<T>* ptr;
            friend class list;
        };

        // CONSTRUCTORS
        list() : m_head(nullptr), m_last(nullptr) {}
        
        explicit list(const Allocator& alloc) : m_alloc(alloc), m_head(nullptr), m_last(nullptr) {}

        list(std::initializer_list<T> init, const Allocator& alloc = Allocator()) 
            : m_alloc(alloc), m_head(nullptr), m_last(nullptr) {
            for (const auto& val : init) push_back(val);
        }

        list(const list& other) : m_alloc(other.m_alloc), m_head(nullptr), m_last(nullptr) {
            for (Node<T>* curr = other.m_head; curr; curr = curr->next) push_back(curr->data);
        }

        list(list&& other) noexcept 
            : m_alloc(std::move(other.m_alloc)), m_head(other.m_head), m_last(other.m_last) {
            other.m_head = nullptr;
            other.m_last = nullptr;
        }

        ~list() { clear(); }

        // ASSIGNMENT
        list& operator=(const list& other) {
            if (this != &other) { list temp(other); swap(temp); }
            return *this;
        }

        void swap(list& other) noexcept {
            std::swap(m_head, other.m_head);
            std::swap(m_last, other.m_last);
            std::swap(m_alloc, other.m_alloc);
        }

        // ITERATORS
        iterator begin() noexcept { return iterator(m_head); }
        iterator end()   noexcept { return iterator(nullptr); }

        // ELEMENT ACCESS
        reference front() { return m_head->data; }
        reference back()  { return m_last->data; }

        // CAPACITY
        bool empty() const noexcept { return m_head == nullptr; }

        // MODIFIERS
        void push_front(const T& val) { 
            m_head = create_node(nullptr, val, m_head); 
            if (m_head->next) m_head->next->prev = m_head;
            if (!m_last) m_last = m_head; 
        }

        void push_back(const T& val) { 
            m_last = create_node(m_last, val, nullptr); 
            if (m_last->prev) m_last->prev->next = m_last;
            if (!m_head) m_head = m_last; 
        }

        void pop_front() {
            if (!m_head) return;
            Node<T>* old = m_head;
            m_head = m_head->next;
            if (m_head) m_head->prev = nullptr; else m_last = nullptr;
            destroy_node(old);
        }

        void pop_back() {
            if (!m_last) return;
            Node<T>* old = m_last;
            m_last = m_last->prev;
            if (m_last) m_last->next = nullptr; else m_head = nullptr;
            destroy_node(old);
        }

        iterator insert_after(iterator pos, const T& val) {
            if (!pos.ptr) return end();
            Node<T>* newNode = create_node(pos.ptr, val, pos.ptr->next);
            if (pos.ptr->next) pos.ptr->next->prev = newNode; else m_last = newNode;
            pos.ptr->next = newNode;
            return iterator(newNode);
        }

        iterator erase_after(iterator pos) {
            if (!pos.ptr || !pos.ptr->next) return end();
            Node<T>* toDelete = pos.ptr->next;
            pos.ptr->next = toDelete->next;
            if (pos.ptr->next) pos.ptr->next->prev = pos.ptr; else m_last = pos.ptr;
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

            while (curr) {
                Node<T>* next_node = curr->next;
                curr->next = nullptr;
                curr->prev = nullptr;

                int i = 0;
                while (i < 64 && bins[i] != nullptr) {
                    curr = merge_internal(bins[i], curr);
                    bins[i] = nullptr;
                    i++;
                }
                bins[i] = curr;
                curr = next_node;
            }

            Node<T>* result = nullptr;
            for (int i = 0; i < 64; i++) {
                result = merge_internal(bins[i], result);
            }

            m_head = result;
            if (m_head) {
                m_head->prev = nullptr;
                Node<T>* p = m_head;
                while (p->next) {
                    p->next->prev = p;
                    p = p->next;
                }
                m_last = p;
            }
        }

        void reverse() noexcept {
            if (!m_head) return;
            Node<T>* curr = m_head;
            Node<T>* temp = nullptr;
            m_last = m_head; 
            while (curr) {
                temp = curr->prev;
                curr->prev = curr->next;
                curr->next = temp;
                curr = curr->prev;
            }
            if (temp) m_head = temp->prev;
        }

    private:
        Node<T>* merge_internal(Node<T>* l1, Node<T>* l2) {
            if (!l1) return l2;
            if (!l2) return l1;

            Node<T> dummy{nullptr, T(), nullptr};
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

        Node<T>* create_node(Node<T>* p, const T& v, Node<T>* n) {
            Node<T>* ptr = m_alloc.allocate(1);
            try { new (ptr) Node<T>{p, v, n}; }
            catch (...) { m_alloc.deallocate(ptr, 1); throw; }
            return ptr;
        }

        void destroy_node(Node<T>* ptr) {
            ptr->~Node<T>();
            m_alloc.deallocate(ptr, 1);
        }

        Allocator m_alloc;
        Node<T>* m_head;
        Node<T>* m_last;
    };

    template <class T, class Alloc>
    inline void swap(list<T, Alloc>& lhs, list<T, Alloc>& rhs) noexcept {
        lhs.swap(rhs);
    }
}