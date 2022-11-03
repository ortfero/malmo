// This file is part of malmo library
// Copyright 2022 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once


#include <functional>
#include <initializer_list>

#include <malmo/list.hpp>


namespace malmo {
    
    
    template<typename T, typename A>
    class ordered_list {
        using adapted = list<T, A>;
        
        adapted list_;
        
    public:
    
        using value_type = T;
        using iterator = list_iterator<T>;
        using const_iterator = list_const_iterator<T>;
    
    
        ordered_list(list_node_pool<T, A>& nodes) noexcept
        : list_{nodes} {
        }
        
        
        ordered_list(list_node_pool<T, A>& nodes, std::initializer_list<T> values) noexcept
        : list_{nodes, values} {
        }
        
        ordered_list(ordered_list const&) = delete;
        ordered_list& operator = (ordered_list const&) = delete;
        
        ordered_list(ordered_list&&) = default;
        ordered_list& operator = (ordered_list&&) = default;
        
        bool empty() const noexcept { return list_.empty(); }
        const_iterator begin() const noexcept { return list_.begin(); }
        const_iterator end() const noexcept { return list_.end(); }
        iterator begin() noexcept { return list_.begin(); }
        iterator end() noexcept { return list_.end(); }
        T const& front() const noexcept { return list_.front(); }
        T& front() noexcept { return list_.front(); }
        T const& back() const noexcept { return list_.back(); }
        T& back() noexcept { return list_.back(); }
        void clear() noexcept { list_.clear(); }
        iterator erase(iterator it) noexcept { return list_.erase(it); }


        template<class Comparator>
        iterator insert(T const& value, Comparator const& comparator) {
            return list_.insert(find_first_unordered(value, comparator), value);
        }
        
        
        template<class Comparator>
        iterator insert(T&& value, Comparator const& comparator) {
            return list_.insert(find_first_unordered(value, comparator), std::move(value));
        }
        
        
        iterator insert(T const& value) {
            return insert(value, std::less<T>{});
        }
        
        
        iterator insert(T&& value) {
            return insert(std::move(value), std::less<T>{});
        }
        
        
        template<class Comparator>
        void reorder(iterator it, Comparator const& comparator) {
            try_reorder_to_left(it, comparator) || try_reorder_to_right(it, comparator);
        }
        
        
        void reorder(iterator it) {
            reorder(it, std::less<T>{});
        }
        
        
        bool operator == (ordered_list const& other) const noexcept {
            return list_ == other.list_;
        }
        
        
        bool operator != (ordered_list const& other) const noexcept {
            return list_ != other.list_;
        }
        
        
    private:
    
        template<class Comparator>
        iterator find_first_unordered(T const& value, Comparator const& comparator) {
            auto it = list_.begin();
            for(; it != list_.end(); ++it)
                if(!comparator(*it, value))
                    return it;
            return it;
        }
        
        
        template<class Comparator>
        bool try_reorder_to_left(iterator it, Comparator const& comparator) {
            if(it == list_.begin())
                return false;
            auto left_it = it; --left_it;
            if(comparator(*left_it, *it))
                return false;
            --left_it;
            auto before_begin = begin();
            --before_begin;
            while(left_it != before_begin && !comparator(*left_it, *it))
                --left_it;
            list_.rearrange(it, ++left_it);
            return true;
        }
        
        
        template<class Comparator>
        bool try_reorder_to_right(iterator it, Comparator const& comparator) {
            auto right_it = it; ++right_it;
            if(right_it == list_.end())
                return false;
            if(comparator(*it, *right_it))
                return false;
            ++right_it;
            while(right_it != list_.end() && !comparator(*it, *right_it))
                ++right_it;
            list_.rearrange(it, right_it);
            return true;
        }
  
    };
    
    
} // namespace malmo
