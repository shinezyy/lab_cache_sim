#ifndef __CACHE_MEM_H__
#define __CACHE_MEM_H__

#include <cstdint>
#include <vector>

class cache_mem {
    private:
        mem_object valid_col;
        mem_object tag_col;

    public:
        cache_mem(int tag_width, int n_raw) { // number of raws
            valid_col = new mem_object(1, n_raw);
            tag_col = new mem_object(tag_width, n_raw);
        }

        bool match(uint32_t 
};

#endif
