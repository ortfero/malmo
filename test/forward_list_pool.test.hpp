#pragma once


#include "doctest.h"

#include <list>
#include <set>
#include <string>

#include <malmo/forward_list_pool.hpp>


TEST_SUITE("forward_list_pool") {
    
    
    SCENARIO("construct and destroy list") {
        auto pool = malmo::forward_list_pool<int>{};
        auto target = pool.create_list();
        REQUIRE(target.empty());
        pool.destroy_list(target);
    }
    
    
    SCENARIO("insert value") {
        auto pool = malmo::forward_list_pool<int>{};
        auto target = pool.create_list();
        auto it = pool.insert_after(target.before_begin(), -1);
        REQUIRE(!target.empty());
        REQUIRE_EQ(*it, -1);
        pool.destroy_list(target);
    }
    
    SCENARIO("erase value") {
        auto pool = malmo::forward_list_pool<int>{};
        auto target = pool.create_list();
        pool.insert_after(target.before_begin(), -1);
        pool.erase_after(target.before_begin());
        REQUIRE(target.empty());
        pool.destroy_list(target);
    }
    
    SCENARIO("iterate values") {
        auto pool = malmo::forward_list_pool<int>{};
        auto target = pool.create_list();
        auto const first = pool.insert_after(target.before_begin(), -1);
        auto const second = pool.insert_after(first, -2);
        pool.insert_after(second, -3);
        auto it = target.begin();
        REQUIRE_NE(it, target.end());
        REQUIRE_EQ(*it, -1);
        ++it;
        REQUIRE_NE(it, target.end());
        REQUIRE_EQ(*it, -2);
        ++it;
        REQUIRE_NE(it, target.end());
        REQUIRE_EQ(*it, -3);
        ++it;
        REQUIRE_EQ(it, target.end());
        pool.destroy_list(target);
    }
    
    SCENARIO("erase value in the middle") {
        auto pool = malmo::forward_list_pool<int>{};
        auto target = pool.create_list();
        auto const first = pool.insert_after(target.before_begin(), -1);
        auto const second = pool.insert_after(first, -2);
        pool.insert_after(second, -3);
        pool.erase_after(first);
        auto it = target.begin();
        REQUIRE_NE(it, target.end());
        REQUIRE_EQ(*it, -1);
        ++it;
        REQUIRE_NE(it, target.end());
        REQUIRE_EQ(*it, -3);
        ++it;
        REQUIRE_EQ(it, target.end());
        pool.destroy_list(target);
    }


    
}
