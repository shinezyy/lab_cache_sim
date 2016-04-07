#ifndef __CACHE_MEM_H__
#define __CACHE_MEM_H__

#include <cstdint>
#include "mem.h"

class cache_direct_map {
    private:
        mem_object *valid_col;
        mem_object *tag_col;
        uint32_t n_raws;

    public:
        cache_direct_map(int tag_width, int n_r);// number of raws

        bool match(uint32_t tag_in, uint32_t addr);

        void invalidata_all();
};

#endif
