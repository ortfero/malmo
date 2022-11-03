#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <forward_list>
#include <map>
#include <numeric>
#include <vector>

#include <absl/container/btree_map.h>

#include <malmo/list.hpp>
#include <malmo/pyramid.hpp>


static constexpr std::int64_t numbers_count = 1'000'000;
static constexpr int id_range = 50;


inline int random_id() noexcept {
    return 1 + std::rand() * (id_range - 1) / RAND_MAX;
}


inline int random_index() noexcept {
    return std::rand() * numbers_count / RAND_MAX;
}


struct data_type {
    int id;
    char buffer[508];
    data_type* next;
    
    data_type(int id): id{id} { }
}; // data_type


int main() {
    
    auto insert_numbers = std::vector<int>(numbers_count);
    std::generate(insert_numbers.begin(), insert_numbers.end(), random_id);
    
    auto erase_numbers = std::vector<int>{numbers_count};
    std::generate(erase_numbers.begin(), erase_numbers.end(), random_id);
    
    std::printf("numbers count: %u, numbers range: 1-%d\n",
        unsigned(insert_numbers.size()), id_range);
    
    using standard_multimap_type = std::multimap<int, data_type>;
    auto standard_map = standard_multimap_type{};
    auto const standard_multimap_start = std::chrono::steady_clock::now();
    for(auto id: insert_numbers)
        standard_map.insert(standard_multimap_type::value_type{id, data_type{id}});
    for(auto id: erase_numbers)
        standard_map.erase(id);
    auto const standard_time = std::chrono::steady_clock::now() - standard_multimap_start;
    std::printf(
        "std::multimap<int, data_type, std::allocator>: %lldms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(standard_time)
            .count());

    using pyramid_multimap_type = std::multimap<int,
                                           data_type,
                                           std::less<int>,
                                           malmo::pyramid<std::pair<const int, data_type>>>;
    auto pyramid_multimap = pyramid_multimap_type{};
    auto const pyramid_start = std::chrono::steady_clock::now();
    for(auto id: insert_numbers) {
        pyramid_multimap.insert(pyramid_multimap_type::value_type{id, data_type{id}});
    }
    for(auto id: erase_numbers)
        pyramid_multimap.erase(id);
    auto const pyramid_time = std::chrono::steady_clock::now() - pyramid_start; 
    std::printf("std::multimap<int, data_type, malmo::pyramid>: %lldms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(pyramid_time).count());
   
    
    using btree_map_type = absl::btree_multimap<int, data_type>;
    auto btree_map = btree_map_type{};
    auto const btree_start = std::chrono::steady_clock::now();
    for(auto id: insert_numbers)
        btree_map.insert(btree_map_type::value_type{id, data_type{id}});
    for(auto id: erase_numbers)
        btree_map.erase(id);
    auto const btree_time = std::chrono::steady_clock::now() - btree_start;
    std::printf(
        "absl::btree_multimap<int, data_type>: %lldms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(btree_time)
            .count());
            
            
    using vector_map_type = std::map<int, std::vector<data_type>>;
    auto vector_map = vector_map_type{};
    auto const vector_map_start = std::chrono::steady_clock::now();
    for(auto id: insert_numbers) {
        auto& vector = vector_map[id];
        vector.push_back(data_type{id});
    }
    for(auto id: erase_numbers)
        vector_map.erase(id);
    auto const vector_map_time = std::chrono::steady_clock::now() - vector_map_start;
    std::printf(
        "std::map<int, std::vector<data_type>>: %lldms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(vector_map_time)
            .count());

    using deque_map_type = std::map<int, std::deque<data_type>>;
    auto deque_map = deque_map_type{};
    auto const deque_map_start = std::chrono::steady_clock::now();
    for(auto id: insert_numbers) {
        auto& deque = deque_map[id];
        deque.push_back(data_type{id});
    }
    for(auto id: erase_numbers)
        deque_map.erase(id);
    auto const deque_map_time = std::chrono::steady_clock::now() - deque_map_start;
    std::printf(
        "std::map<int, std::deque<data_type>>: %lldms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(deque_map_time)
            .count());
            
    using pool_type = malmo::list_node_pool<data_type>;
    using list_type = malmo::list<data_type>;
    using list_map_type = std::map<int,
                                   list_type,
                                   std::less<int>,
                                   malmo::pyramid<std::pair<const int, list_type>>>;
    auto list_map = list_map_type{};
    auto pool = pool_type{};
    auto const list_map_start = std::chrono::steady_clock::now();
    for(auto id: insert_numbers) {
        auto const emplaced = list_map.try_emplace(id, pool);
        auto& list = emplaced.first->second;
        list.push_back(data_type{id});
    }
    for(auto id: erase_numbers)
        list_map.erase(id);
    auto const list_map_time = std::chrono::steady_clock::now() - list_map_start;
    std::printf(
        "std::map<int, malmo::list<data_type>, malmo::pyramid>: %lldms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(list_map_time)
            .count()); 


    return 0;
}
