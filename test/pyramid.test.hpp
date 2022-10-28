#pragma once


#include "doctest.h"

#include <list>
#include <set>
#include <string>

#include <malmo/pyramid.hpp>


TEST_SUITE("pyramid") {
    
    
    SCENARIO("insert and remove") {
        using target_type = std::set<std::string,
                                     std::less<std::string>,
                                     malmo::pyramid<std::string>>;
        auto target = target_type{};
        for(auto c = 'a'; c != 'z' + 1; ++c)
            target.insert(std::string{c});
        for(auto c = 'a'; c != 'z' + 1; ++c)
            target.erase(std::string{c});
        REQUIRE(target.empty());
    }
    
    
}
