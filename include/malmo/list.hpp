// This file is part of malmo library
// Copyright 2022 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once


#include <initializer_list>
#include <type_traits>

#include <malmo/pyramid.hpp>


namespace malmo {
    
    
    struct list_node_none { };
    
    
    template<typename T>
    struct list_node {
        union {
            list_node_none none;
            T item;
        };
        list_node* next;
        list_node* previous;
        
        list_node() {}
    }; // list_node
    
    
    template<typename T, class A = pyramid<list_node<T>>>
    class list_node_pool {
        
        static_assert(std::is_same_v<typename A::value_type, list_node<T>>,
            "allocator for list_node<T> is expected");
        
        A allocator_;
        
    public:
    
        using value_type = T;
        using allocator_type = A;
        
    
        list_node_pool() = default;
        list_node_pool(list_node_pool const&) = default;
        list_node_pool& operator = (list_node_pool const&) = default;
        list_node_pool(list_node_pool&&) = default;
        list_node_pool& operator = (list_node_pool&&) = default;
        
        
        list_node<T>* create(T const& item) {
            auto* node = allocator_.allocate();
            new(&node->item) T(item);
            return node;
        }
        
        
        list_node<T>* create(T&& item) {
            auto* node = allocator_.allocate();
            new(&node->item) T(std::move(item));
            return node;
        }
        
        
        void destroy(list_node<T>* node) noexcept {
            node->item.~T();
            allocator_.deallocate(node);
        }
        
    }; // list_node_pool
    
    
    template<typename T, class A>
    class list;
    
   
    template<typename T>
    class list_iterator {
    template<typename, class A> friend class list;
    
       list_node<T>* node_;
            
       list_iterator(list_node<T>* node)
            : node_{node} { }
            
    public:
        
        list_iterator(list_iterator const&) = default;
        list_iterator& operator = (list_iterator const&) = default;
        
        T& operator * () const noexcept {
            return node_->item;
        }
        
        
        T* operator -> () const noexcept {
            return &node_->item;
        }

        
        list_iterator& operator ++ () noexcept {
            node_ = node_->next;
            return *this;
        }
        
        
        list_iterator operator ++ (int) noexcept {
            auto const last = *this;
            node_ = node_->next;
            return last;
        }
        
        
        list_iterator& operator -- () noexcept {
            node_ = node_->previous;
            return *this;
        }
        
        
        list_iterator operator -- (int) noexcept {
            auto const last = *this;
            node_ = node_->previous;
            return last;
        }
        
        
        bool operator == (list_iterator other) const noexcept {
            return node_ == other.node_;
        }
        
        
        bool operator != (list_iterator other) const noexcept {
            return node_ != other.node_;
        }

    }; // list_iterator
    
    
    template<typename T>
    class list_const_iterator {
    template<typename, class A> friend class list;
    
       list_node<T> const* node_;
            
       list_const_iterator(list_node<T> const* node)
            : node_{node} { }
            
    public:
        
        list_const_iterator(list_const_iterator const&) = default;
        list_const_iterator& operator = (list_const_iterator const&) = default;
        
        explicit list_const_iterator(list_iterator<T> const& it) noexcept
        : node_{it.node_} {
        }
        
        
        T const& operator * () const noexcept {
            return node_->item;
        }
        
        
        T const* operator -> () const noexcept {
            return &node_->item;
        }

        
        list_const_iterator& operator ++ () noexcept {
            node_ = node_->next;
            return *this;
        }
        
        
        list_const_iterator operator ++ (int) noexcept {
            auto const last = *this;
            node_ = node_->next;
            return last;
        }
        
        
        list_const_iterator& operator -- () noexcept {
            node_ = node_->previous;
            return *this;
        }
        
        
        list_const_iterator operator -- (int) noexcept {
            auto const last = *this;
            node_ = node_->previous;
            return last;
        }
        
        
        bool operator == (list_const_iterator other) const noexcept {
            return node_ == other.node_;
        }
        
        
        bool operator != (list_const_iterator other) const noexcept {
            return node_ != other.node_;
        }

    }; // list_const_iterator
    
    
    template<typename T, typename A = pyramid<list_node<T>>>
    class list {
        
        static_assert(std::is_same_v<typename A::value_type, list_node<T>>,
            "allocator for list_node<T> is expected");
        
        list_node<T> head_;
        list_node_pool<T, A>* nodes_;
        
    public:
        using value_type = T;
        using iterator = list_iterator<T>;
        using const_iterator = list_const_iterator<T>;
        
        
        list() noexcept
        : nodes_{nullptr} {
            reset();
        }
            
            
        list(list_node_pool<T, A>& nodes) noexcept
        : nodes_{&nodes} {
            reset();
        }
        
        
        list(list_node_pool<T, A>& nodes, std::initializer_list<T> values)
        : nodes_{&nodes} {
            reset();
            for(auto const& value: values)
                push_back(value);
        }
        
        
        ~list() {
            cleanup();
        }
        
        
        list(list const&) = delete;
        list& operator = (list const&) = delete;
        
        list(list&& other) noexcept {
            transfer_from(other);
        }
        
        
        list& operator = (list&& other) noexcept {
            cleanup();
            transfer_from(other);
            return *this;
        }
        
        
        bool has_pool() const noexcept {
            return nodes_ != nullptr;
        }
        
        
        void set_pool(list_node_pool<T, A>& nodes) noexcept {
            clear();
            nodes_ = &nodes;
        }
                
        
        bool empty() const noexcept {
            return head_.next == &head_;
        }
        
        
        const_iterator begin() const noexcept {
            return const_iterator{head_.next};
        }
        
        
        const_iterator end() const noexcept {
            return const_iterator{&head_};
        }
     
        
        iterator begin() noexcept {
            return iterator{head_.next};
        }
        
        
        iterator end() noexcept {
            return iterator{&head_};
        }
        
        
        T const& front() const noexcept {
            return head_.next->item;
        }
        
        
        T& front() noexcept {
            return head_.next->item;
        }
        
        
        T const& back() const noexcept {
            return head_.previous->item;
        }
        
        
        T& back() noexcept {
            return head_.previous->item;
        }
        
        
        void clear() noexcept {
            cleanup();
            reset();
        }
        
        
       iterator insert(iterator before, T const& value) {
            auto* node = nodes_->create(value);
            return insert_node_before(before.node_, node);
        }
        
        
        iterator insert(iterator before, T&& value) {
            auto* node = nodes_->create(std::move(value));
            return insert_node_before(before.node_, node);
        }
        
        
        iterator erase(iterator it) noexcept {
            return erase_node(it.node_);
        }
        
        
        void push_back(T const& value) {
            insert(end(), value);
        }
        
        
        void push_back(T&& value) {
            insert(end(), std::move(value));
        }
        
        
        void pop_back() noexcept {
            erase_node(head_.previous);
        }
        
        
        void rearrange(iterator source, iterator before) {
            auto* source_previous = source.node_->previous;
            auto* source_next = source.node_->next;
            source_previous->next = source_next;
            source_next->previous = source_previous;
            auto* new_previous = before.node_->previous;
            new_previous->next = source.node_;
            before.node_->previous = source.node_;
            source.node_->previous = new_previous;
            source.node_->next = before.node_;
        }
        
        
        bool operator == (list const& other) const noexcept {
            auto it1 = begin(), it2 = other.begin();
            for(; it1 != end() && it2 != other.end();
                ++it1, ++it2) {
                    if(*it1 != *it2)
                        return false;
            }
            return it1 == end() && it2 == other.end();
        }
        
        
        bool operator != (list const& other) const noexcept {
            return !(*this == other);
        }

        
    private:
    
        void transfer_from(list& other) {
            nodes_ = other.nodes_;
            if(other.head_.next == &other.head_)
                head_.next = &head_;
            else
                head_.next = other.head_.next;
            if(other.head_.previous == &other.head_)
                head_.previous = &head_;
            else
                head_.previous = other.head_.previous;
            other.reset();
        }
    
        void cleanup() noexcept {
            if(!nodes_)
                return;
            auto* node = head_.next;
            while(node != &head_) {
                auto* disposable = node;
                node = node->next;
                nodes_->destroy(disposable);
            }
        }
        
        
        void reset() noexcept {
            head_.next = &head_;
            head_.previous = &head_;
        }
        
        
        iterator insert_node_before(list_node<T>* node, list_node<T>* new_node) {
            auto* previous = node->previous;
            new_node->next = node;
            new_node->previous = previous;
            previous->next = new_node;
            node->previous = new_node;
            return iterator{new_node};
        }
        
        
        iterator erase_node(list_node<T>* node) {
            auto* next_node = node->next;
            auto* previous_node = node->previous;
            next_node->previous = previous_node;
            previous_node->next = next_node;
            nodes_->destroy(node);
            return iterator{next_node};
        }
    }; // list

    
} // namespace malmo
