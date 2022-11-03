#pragma once


#include "doctest.h"

#include <malmo/list.hpp>


TEST_SUITE("list") {
    
    SCENARIO("construct empty list") {
        auto pool = malmo::list_node_pool<int>{};
        auto target = malmo::list{pool};
        REQUIRE(target.empty());
        REQUIRE_EQ(target.begin(), target.end());
    }
    
    
    SCENARIO("construct from other empty list") {
        auto pool = malmo::list_node_pool<int>{};
        auto other = malmo::list{pool};
        auto target = malmo::list{std::move(other)};
        REQUIRE(target.empty());
        REQUIRE_EQ(target.begin(), target.end());
    }
    
    
    SCENARIO("insert value and clear") {
        auto pool = malmo::list_node_pool<int>{};
        auto target = malmo::list{pool};
        auto it = target.insert(target.end(), -1);
        REQUIRE(!target.empty());
        REQUIRE_EQ(*it, -1);
        target.clear();
        REQUIRE(target.empty());
        REQUIRE_EQ(target.begin(), target.end());
    }
    
    
    SCENARIO("erase iterator") {
        auto pool = malmo::list_node_pool<int>{};
        auto target = malmo::list{pool};
        auto it = target.insert(target.end(), -1);
        it = target.erase(it);
        REQUIRE(target.empty());
        REQUIRE_EQ(it, target.end());
        target.clear();
    }
    
    
    SCENARIO("push back and iterate values") {
        auto pool = malmo::list_node_pool<int>{};
        auto target = malmo::list{pool};
        target.push_back(1);
        target.push_back(2);
        target.push_back(3);
        auto it = target.begin();
        REQUIRE_NE(it, target.end());
        REQUIRE_EQ(*it, 1);
        ++it;
        REQUIRE_NE(it, target.end());
        REQUIRE_EQ(*it, 2);
        ++it;
        REQUIRE_NE(it, target.end());
        REQUIRE_EQ(*it, 3);
        ++it;
        REQUIRE_EQ(it, target.end());
        it = target.end();
        --it;
        REQUIRE_EQ(*it, 3);
        --it;
        REQUIRE_EQ(*it, 2);
        --it;
        REQUIRE_EQ(*it, 1);
        REQUIRE_EQ(it, target.begin());
        target.clear();
    }
    
    
    SCENARIO("equality") {
        auto pool = malmo::list_node_pool<int>{};
        auto target_x = malmo::list{pool, {1, 2, 3}};
        auto target_y = malmo::list{pool, {1, 2, 3}};
        auto target_z = malmo::list{pool, {1, 2, 3, 4}};
        REQUIRE_EQ(target_x, target_y);
        REQUIRE_NE(target_y, target_z);
        target_x.clear();
        target_y.clear();
        target_z.clear();
    }
    
    
    SCENARIO("erase iterator in the middle") {
        auto pool = malmo::list_node_pool<int>{};
        auto target = malmo::list{pool, {1, 2, 3}};
        auto it = target.begin();
        it = target.erase(++it);
        REQUIRE_EQ(*it, 3);
        ++it;
        REQUIRE_EQ(it, target.end());
        target.clear();
    }
    
    
    SCENARIO("rearrange") {
        auto pool = malmo::list_node_pool<int>{};
        auto target = malmo::list{pool, {1, 2, 3}};
        target.rearrange(target.begin(), target.end());
        REQUIRE_EQ(target, malmo::list{pool, {2, 3, 1}});
        target.clear();
    }
    
    
    SCENARIO("rearrange to the same place") {
        auto pool = malmo::list_node_pool<int>{};
        auto target = malmo::list{pool, {1, 2}};
        target.rearrange(--target.end(), target.end());
        REQUIRE_EQ(target, malmo::list{pool, {1, 2}});
        target.clear();
    }
    
    
}
