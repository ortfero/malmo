# malmo
C++ memory allocator for map and list


## Features and limitations

* Next page size is the factor of previous one (16 by default).
  So to store 65536 elements we need 4 pages (16, 256, 4096 and 65536 elements).
  
* Faster than std::allocator
  
* Most of the data stored at continuous memory chunk.

* No memory fragmentation as memory allocated by fixed size chunks.

* Suitable for map, list, forward_list (single item allocation).
  Not suitable for vector, unordered_map, flat_map (array allocation).

  
## Usage

Drop `malmo` directory at your include path.


## Snippets


### Using pyramid allocator for std::map

```cpp
#include <map>
#include <malmo/pyramid.hpp>

...

using map = std::map<int,
                     int,
                     std::less<int>,
                     malmo::pyramid<std::pair<int const, int>>>;
```


### Using lists with common pool of nodes

```cpp
#include <malmo/list.hpp>

...

auto pool = malmo::list_node_pool<int>{};
auto list1 = malmo::list{pool};
auto list2 = malmo::list{pool}; // list1 and list2 have common pool of nodes
...
list1.clear(); list2.clear(); // destroy nodes before pool destruction
```


## Benchmark

Insert and remove 1'000'000 random numbers from 1 to 50:

| Data type                                  | Time, ms | Percent |
|:-------------------------------------------|---------:|--------:|
| multimap                                   |      707 |    100% |
| multimap with pyramid allocator            |      514 |     72% |
| abseil btree_multimap                      |     3940 |    557% |
| map of vectors                             |      474 |     67% |
| map of deques                              |      448 |     63% |
| map of custom lists with pyramid allocator |      193 |     27% |

Ryzen 4800u, Windows 11, gcc 11.2.0
