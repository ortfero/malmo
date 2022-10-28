# malmo
C++ memory allocator for map and list


## Features and limitations

* Next page size is the factor of previous one (16 by default).
  So to store 65536 elements we need 4 pages (16, 256, 4096 and 65536 elements).
  
* Faster than std::allocator
  
* Most of the data stored at continuous memory chunk.

* No memory fragmnetation as memory allocated by fixed size chunks.

* Suitable for map, list, forward_list (single item allocation).
  Not suitable for vector, unordered_map, flat_map (array allocation).

  
## Usage

Drop `malmo` directory at your include path.


## Snippets

```
#include <map>
#include <malmo/pyramid.hpp>

...

using map = std::map<int,
                     int,
                     std::less<int>,
                     malmo::pyramid<std::pair<int const, int>>>;
```


## Benchmark

Insert and remove 1'000'000 random numbers from 1 to 50:

| Data type                       | Time, ms | Percent |
+---------------------------------+---------:+--------:+
| multimap                        |      707 |    100% |
| multimap with pyramid allocator |      514 |     72% |
| abseil btree_multimap           |     3940 |    557% |
| map of vectors                  |      474 |     67% |
| map of deques                   |      448 |     63% |
| map of custom lists             |      193 |     27% |

Ryzen 4800u, Windows 11, gcc 11.2.0