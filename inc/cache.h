#ifndef __CACHE_H__
#define __CACHE_H__

#include <vector>
#include <cstdint>
#include "cache_mem.h"

#define LRU_LEN 4

class cache {
    private: 
        std :: vector<cache_direct_map *> simple_cache;
        uint32_t rand_sel; 
    public:
        cache(uint32_t size, uint32_t line_size, uint32_t assoc, bool en_lru);
        void write(uint32_t addr);
        void read(uint32_t addr);
};

#endif
