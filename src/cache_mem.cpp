#include "cache_mem.h"

cache_direct_map :: cache_direct_map(int tag_width, int n_r) { // number of raws
    valid_col = new mem_object(1, n_r);
    tag_col = new mem_object(tag_width, n_r);
    n_raws = n_r;
}

bool cache_direct_map :: match(uint32_t tag_in, uint32_t addr) {
    uint32_t valid = valid_col->read(addr);
    uint32_t tag_match = tag_col->read(addr) == tag_in;
    return valid & tag_match;
}

void cache_direct_map :: invalidata_all() {
    uint32_t i;
    for(i = 0; i < n_raws; i++) {
        valid_col->write(i, 0);
    }
}
