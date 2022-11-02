// This file is part of malmo library
// Copyright 2022 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once


#include <type_traits>

#include <malmo/pyramid.hpp>


namespace malmo {
     

    struct forward_list_node_base {
        forward_list_node_base* link;
    }; // forward_list_node_base;
    
    
    template<typename T>
    struct forward_list_node: forward_list_node_base {
        T item;
    }; // forward_list_node
    
    
    template<typename T>
    class forward_list;
    
    template<typename T, class A>
    class forward_list_pool;
    
    
    template<typename T>
    class forward_list_iterator {
    friend class forward_list<T>;
    template<typename, class A> friend class forward_list_pool;
    
       forward_list_node_base* node_;
            
       forward_list_iterator(forward_list_node_base* node)
            : node_{node} { }
            
    public:
        
        forward_list_iterator(forward_list_iterator const&) = default;
        forward_list_iterator& operator = (forward_list_iterator const&) = default;
        
        T& operator * () const noexcept {
            return static_cast<forward_list_node<T>*>(node_)->item;
        }
        
        
        T* operator -> () const noexcept {
            return &static_cast<forward_list_node<T>*>(node_)->item;
        }

        
        forward_list_iterator& operator ++ () noexcept {
            node_ = node_->link;
            return *this;
        }
        
        
        forward_list_iterator operator ++ (int) noexcept {
            auto const last = *this;
            node_ = node_->link;
            return last;
        }
        
        
        bool operator == (forward_list_iterator other) const noexcept {
            return node_ == other.node_;
        }
        
        
        bool operator != (forward_list_iterator other) const noexcept {
            return node_ != other.node_;
        }

    }; // forward_list_iterator


    template<typename T>
    class forward_list_const_iterator {
    friend class forward_list<T>;
    template<typename, class A> friend class forward_list_pool;
    
       forward_list_node_base const* node_;
            
       forward_list_const_iterator(forward_list_node_base* node)
            : node_{node_} { }
            
    public:
        
        forward_list_const_iterator(forward_list_const_iterator const&) = default;
        forward_list_const_iterator& operator = (forward_list_const_iterator const&) = default;
        
        T const& operator * () const noexcept {
            return static_cast<forward_list_node<T> const*>(node_)->item;
        }
        
        
        T const* operator -> () const noexcept {
            return &static_cast<forward_list_node<T> const*>(node_)->item;
        }

        
        forward_list_const_iterator& operator ++ () noexcept {
            node_ = node_->link;
            return *this;
        }
        
        
        forward_list_const_iterator operator ++ (int) noexcept {
            auto const last = *this;
            node_ = node_->link;
            return last;
        }
        
        
        bool operator == (forward_list_const_iterator other) const noexcept {
            return node_ == other.node_;
        }
        
        
        bool operator != (forward_list_const_iterator other) const noexcept {
            return node_ != other.node_;
        }

    }; // forward_list_const_iterator
    
    
    template<typename T>
    class forward_list {
    template<typename, typename A> friend class forward_list_pool;
        
        forward_list_node_base head_;
        
    public:
        using value_type = T;
        using iterator = forward_list_iterator<T>;
        using const_iterator = forward_list_const_iterator<T>;
            
            
        forward_list() {
            head_.link = nullptr;
        }
        
        
        forward_list(forward_list const&) = delete;
        forward_list& operator = (forward_list const&) = delete;
        
        forward_list(forward_list&& other): head_{other.head_} {
            other.head_.link = nullptr;
        }
        
        
        forward_list& operator = (forward_list&& other) {
            head_ = other.head_;
            other.head_.link = nullptr;
            return *this;
        }

        
        ~forward_list() {
        }
        
        
        bool empty() const noexcept {
            return head_.link == nullptr;
        }
        
        
        const_iterator before_begin() const noexcept {
            return const_iterator{&head_};
        }
        
        
        const_iterator begin() const noexcept {
            return const_iterator{head_.link};
        }
        
        
        const_iterator end() const noexcept {
            return const_iterator{nullptr};
        }

        
        iterator before_begin() noexcept {
            return iterator{&head_};
        }
        
        
        iterator begin() noexcept {
            return iterator{head_.link};
        }
        
        
        iterator end() noexcept {
            return iterator{nullptr};
        }
    }; // forward_list

    
    
    template<typename T, class A = pyramid<forward_list_node<T>>>
    class forward_list_pool {
        A allocator_;
        
    public:
    
        static_assert(std::is_same_v<typename A::value_type, forward_list_node<T>>,
            "forward_list_node<T> allocator is expected");
        using value_type = T;
        using allocator_type = A;
        using const_iterator = forward_list_const_iterator<T>;
        using iterator = forward_list_iterator<T>;
        
    
        forward_list_pool() = default;
        forward_list_pool(forward_list_pool const&) = default;
        forward_list_pool& operator = (forward_list_pool const&) = default;
        forward_list_pool(forward_list_pool&&) = default;
        forward_list_pool& operator = (forward_list_pool&&) = default;
        
        forward_list<T> create_list() {
            return forward_list<T>{};
        }
                
        
        void destroy_list(forward_list<T>& list) noexcept {
            auto* node = list.head_.link;
            while(node != nullptr) {
                auto* disposable = static_cast<forward_list_node<T>*>(node);
                node = node->link;
                disposable->item.~T();
                allocator_.deallocate(disposable);
            }
            list.head_.link = nullptr;
        }
        
        
        iterator insert_after(iterator it, T const& value) {
            auto* node = allocator_.allocate();
            new(&node->item) T(value);
            node->link = it.node_->link;
            it.node_->link = node;
            return iterator{node};
        }
        
        
        iterator insert_after(iterator it, T&& value) {
            auto* node = allocator_.allocate();
            new(&node->item) T(std::move(value));
            node->link = it.node_->link;
            it.node_->link = node;
            return iterator{node};
        }
        
        
        iterator insert(forward_list<T>& list, T const& value) {
            auto previous_it = list.before_begin();
            for(auto it = list.begin();
                it != list.end();
                ++previous_it, ++it) {
                    if(*it >= value)
                        break;
            }
            return insert_after(previous_it, value);
        }
        
        
        iterator erase_after(iterator it) noexcept {
            auto* after_node = static_cast<forward_list_node<T>*>(it.node_->link);
            auto* next_after_node = after_node->link;
            it.node_->link = next_after_node;
            after_node->item.~T();
            allocator_.deallocate(after_node);
            return iterator{next_after_node};
        }
        
        
        bool erase(forward_list<T>& list, T const& value) noexcept {
            for(auto previous_it = list.before_begin(), it = list.begin();
                it != list.end();
                ++previous_it, ++it) {
                    if(*it != value)
                        continue;
                    erase_after(previous_it);
                    return true;
            }
        }
        
        
        template<class Comparator>
        bool erase(forward_list<T>& list, T const& value, Comparator&& comparator) noexcept {
            for(auto previous_it = list.before_begin(), it = list.begin();
                it != list.end();
                ++previous_it, ++it) {
                    if(comparator(*it, value))
                        continue;
                    erase_after(previous_it);
                    return true;
            }
        }
        
    }; // forward_list_pool
    
} // namespace malmo
