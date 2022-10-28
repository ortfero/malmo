// This file is part of malmo library
// Copyright 2022 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once


#include <cassert>
#include <cstddef>
#include <cstring>
#include <new>
#include <type_traits>


namespace malmo {
    
    
    namespace detail {
        
        template<typename T>
        union pyramid_node {
            pyramid_node* link;
            T item;
        }; // pyramid_node
       
        
        using pyramid_size_type = std::size_t;
        
        
        template<typename T>
        struct pyramid_page {
            pyramid_page* link;
            pyramid_node<T> nodes[1];
        }; // pyramid_page
        
        
    } // namespace detail
    
    
    template<typename T, detail::pyramid_size_type F = 16>
    class pyramid {
        
        detail::pyramid_page<T>* page_;
        detail::pyramid_node<T>* node_;
        detail::pyramid_size_type page_capacity_;
        detail::pyramid_size_type node_index_;
        detail::pyramid_size_type next_page_estimate_;
        
        
    public:
    
        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;
    
        using size_type = detail::pyramid_size_type;
        using difference_type = std::ptrdiff_t;
        using value_type = T;

        template<typename U> struct rebind {
            using other = pyramid<U>;
        };
               
        static size_type constexpr factor = F;
        
        
        pyramid() noexcept {
            init();
        }
        
        
        ~pyramid() {
            clear();
        }
        
        
        pyramid(pyramid const&) noexcept {
            init();
        }


        template<typename U>
        constexpr pyramid(pyramid<U> const&) noexcept {
            init();
        }
        
        
        pyramid& operator = (pyramid const&) noexcept {
            init();
            return *this;
        }
        
        
        pyramid(pyramid&& other) noexcept {
            move_from(std::move(other));
        }
        
        
        pyramid& operator = (pyramid&& other) noexcept {
            clear();
            move_from(std::move(other));
            return *this;
        }
        
        
        T* allocate() {
            if(node_) {
                auto* item = &node_->item;
                node_ = node_->link;
                return item;
            }
            if(node_index_ == page_capacity_) {
                page_capacity_ = next_page_estimate_;
                next_page_estimate_ *= F;
                auto const header_size = sizeof(detail::pyramid_page<T>);
                auto const nodes_size = (page_capacity_ - 1) * sizeof(detail::pyramid_node<T>);
                auto* page = static_cast<detail::pyramid_page<T>*>(std::malloc(header_size + nodes_size));
                if(!page)
                    throw std::bad_alloc{};
                page->link = page_;
                page_ = page;
                node_index_ = 0;
            }
            return &page_->nodes[node_index_++].item;

        }
        
        
        T* allocate(size_type) {
            return allocate();
        }
        
        
        void deallocate(T* p) {
            auto* node = reinterpret_cast<detail::pyramid_node<T>*>(p);
            node->link = node_;
            node_ = node;
        }
        
        
        void deallocate(T* p, size_type) {
            deallocate(p);
        }
        
        
    private:
    
        void init() noexcept {
            page_ = nullptr;
            node_ = nullptr;
            page_capacity_ = 0;
            node_index_ = 0;
            next_page_estimate_ = factor;
        }
        
        
        void clear() noexcept {
            auto* page = page_;
            while(page != nullptr) {
                auto* disposable = page;
                page = page->link;
                std::free(disposable);
            }
            init();
        }
        
        
        void move_from(pyramid&& other) noexcept {
            page_ = other.page_;
            node_ = other.node_;
            page_capacity_ = other.page_capacity_;
            node_index_ = other.node_index_;
            next_page_estimate_ = other.next_page_estimate_;
            other.init();
        }

    }; // pyramid
    
    
} // namespace malmo
