#ifndef __CACHE_H__
#define __CACHE_H__

#include <vector>
#include <cstdint>
#include "cache_mem.h"

#define LRU_LEN 4
#define BUS_WIDTH 32

class cache {
    private: 
        std :: vector<cache_direct_map *> simple_cache;
        uint32_t rand_sel; 
        uint32_t tag_width, index_width, offset_width;
        bool lru;

        uint32_t addr_to_tag(uint32_t addr);
        uint32_t addr_to_index(uint32_t addr);
    public:
        cache(uint32_t size, uint32_t line_size, int32_t assoc, bool en_lru);

        bool write(uint32_t addr, bool cmp);

        bool read(uint32_t addr);
};

#endif
