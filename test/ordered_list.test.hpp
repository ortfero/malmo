#pragma once


#include "doctest.h"

#include <malmo/ordered_list.hpp>


TEST_SUITE("ordered_list") {
    
    
    SCENARIO("insert") {
        auto pool = malmo::list_node_pool<int>{};
        auto target = malmo::ordered_list{pool};
        target.insert(1);
        REQUIRE_EQ(target, malmo::ordered_list{pool, {1}});
        target.insert(4);
        REQUIRE_EQ(target, malmo::ordered_list{pool, {1, 4}});
        target.insert(2);
        REQUIRE_EQ(target, malmo::ordered_list{pool, {1, 2, 4}});
        target.insert(3);
        REQUIRE_EQ(target, malmo::ordered_list{pool, {1, 2, 3, 4}});
        target.clear();
    }
    
    
    SCENARIO("reorder") {
        auto pool = malmo::list_node_pool<int>{};
        auto target = malmo::ordered_list{pool};
        auto it = target.insert(3);
        target.reorder(it);
        REQUIRE_EQ(target, malmo::ordered_list{pool, {3}});
        it = target.insert(5);
        target.reorder(it);
        REQUIRE_EQ(target, malmo::ordered_list{pool, {3, 5}});
        it = target.insert(4);
        target.reorder(it);
        REQUIRE_EQ(target, malmo::ordered_list{pool, {3, 4, 5}});
        *it = 1;
        target.reorder(it);
        REQUIRE_EQ(target, malmo::ordered_list{pool, {1, 3, 5}});
        it = target.insert(2);
        *it = 4;
        target.reorder(it);
        REQUIRE_EQ(target, malmo::ordered_list{pool, {1, 3, 4, 5}});
        target.clear();
    }
    
}
