#include "direct_mapped.h"
#include "debug.h"
#include <cassert>

cache_direct_map :: cache_direct_map(uint32_t tag_width, uint32_t n_r, uint32_t age_width) {
    valid_col = new mem_object(1, n_r);
    tag_col = new mem_object(tag_width, n_r);
    if(age_width != 0) {
        age_col = new mem_object(age_width, n_r);
    }
    else {
        age_col = nullptr;
    }
    n_raws = n_r;
}

bool cache_direct_map :: match(uint32_t tag_in, uint32_t addr) {
    assert(addr < n_raws);
    uint32_t valid = valid_col->read(addr);
    uint32_t tag_match = tag_col->read(addr) == tag_in;
    if(valid & tag_match) {
        if(age_col != nullptr) {
            age_col->write(addr, ~0);
            printf("recent use: %u\n", get_recent_use(addr));
        }
        return true;
    }
    else {
        return false;
    }
}

bool cache_direct_map :: match_without_aging(uint32_t tag_in, uint32_t addr) {
    assert(addr < n_raws);
    uint32_t valid = valid_col->read(addr);
    uint32_t tag_match = tag_col->read(addr) == tag_in;
    if(valid & tag_match) {
        return true;
    }
    else {
        return false;
    }
}

void cache_direct_map :: invalidate_all() {
    uint32_t i;
    for(i = 0; i < n_raws; i++) {
        valid_col->write(i, 0);
    }
}

void cache_direct_map :: write(uint32_t tag_in, uint32_t addr) {
    assert(addr < n_raws);
    tag_col->write(addr, tag_in);
    valid_col->write(addr, 1); // 1 for valid
    if(age_col != nullptr) {
        age_col->write(addr, ~0);
    }
}

void cache_direct_map :: time_pass_by() {
    if(age_col != nullptr){
        uint32_t i;
        for(i = 0; i < n_raws; i++) {
            if(valid_col->read(i) && age_col->read(i)) { // > 0 and valid
                age_col->write(i, age_col->read(i) - 1);
            }
        }
    }
}

uint32_t cache_direct_map :: get_recent_use(uint32_t addr) {
    assert(addr < n_raws);
    assert(age_col != nullptr);
    return age_col->read(addr);
}

uint32_t cache_direct_map :: get_tag(uint32_t addr) {
    assert(addr < n_raws);
    return tag_col->read(addr);
}

uint32_t cache_direct_map :: get_valid(uint32_t addr) {
    assert(addr < n_raws);
    return valid_col->read(addr);
}

void cache_direct_map :: invalidate_a_line(uint32_t addr) {
    assert(addr < n_raws);
    valid_col->write(addr, 0);
}
