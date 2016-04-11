#include <cstdlib>
#include "debug.h"
#include "cache.h"

using namespace std;

static uint32_t log2(uint32_t x) {
    if(x != 1) {
        return 1 + log2(x >> 1);
    }
    else {
        return 0;
    }
}

// count in byte
cache :: cache(uint32_t size, uint32_t line_size, int32_t assoc, bool en_lru) {
    uint32_t age_width, n_raws_all, n_ways, n_raws_in_way;
    //uint32_t offset_width, index_width, tag_width;

    if(en_lru) { 
        age_width = LRU_LEN;
        lru = true;
    }
    else {
        age_width = 0;
        lru = false;
    }

    n_raws_all = size/line_size;

    if(assoc == -1) { //fully
        n_ways = n_raws_all;
    }
    else {
        n_ways = assoc;
    }
    n_raws_in_way = n_raws_all / n_ways; // 1 for fully assoc...

    index_width = log2(n_raws_in_way);
    offset_width = log2(line_size);
    tag_width = BUS_WIDTH - index_width - offset_width;
    
    // init members
    rand_sel = 0;
    uint32_t i;
    for(i = 0; i < n_ways; i++) {
        simple_cache.push_back(new cache_direct_map(tag_width, n_raws_in_way, age_width));
        simple_cache[i]->invalidate_all();
    }
}

bool cache :: write(uint32_t addr, bool cmp, uint32_t *victim) {
    if(cmp) {
        for(auto it = simple_cache.begin(); it != simple_cache.end(); it++) {
            (*it)->time_pass_by();
        }

        for(auto it = simple_cache.begin(); it != simple_cache.end(); it++) {
            if((*it)->match(addr_to_tag(addr), addr_to_index(addr))) {
                // do nothing;
                return true;
            }
        }
        return false;
    }
    else { // replacement
        uint32_t victim_idx = 0;
        if(lru) {
            uint32_t min = simple_cache[0]->get_recent_use(addr_to_index(addr));
            uint32_t min_idx = 0;
            uint32_t i;
            for(i = 1; i < simple_cache.size(); i++) {
                if(simple_cache[i]->get_recent_use(addr_to_index(addr)) < min) {
                    min = simple_cache[i]->get_recent_use(addr_to_index(addr));
                    min_idx = i;
                }
            }
            victim_idx = min_idx;
        }
        else {
            rand_sel += 1;
            if(rand_sel >= simple_cache.size()){
                rand_sel -= simple_cache.size();
            }
            victim_idx = rand_sel;
        }

        simple_cache[victim_idx]->write(addr_to_tag(addr), addr_to_index(addr));
        if(victim != nullptr) {
            uint32_t victim_tag = simple_cache[rand_sel]->get_tag(addr_to_index(addr));
            *victim = (victim_tag << (index_width + offset_width))
                | (victim_idx << index_width);
        }

        return true;
    }
}

uint32_t cache :: addr_to_tag(uint32_t addr) {
    return addr >> (BUS_WIDTH - tag_width);
}


uint32_t cache :: addr_to_index(uint32_t addr) {
    return index_width ? (addr << tag_width) >> (BUS_WIDTH - index_width) : 0;
}

bool cache :: read(uint32_t addr) {
    for(auto it = simple_cache.begin(); it != simple_cache.end(); it++) {
        (*it)->time_pass_by();
    }
    for(auto it = simple_cache.begin(); it != simple_cache.end(); it++) {
        if((*it)->match(addr_to_tag(addr), addr_to_index(addr))) {
            return true;
        }
    }
    return false;
}

void cache :: invalidate_all() {
    uint32_t i;
    for(i = 0; i < simple_cache.size(); i++) {
        simple_cache[i]->invalidate_all();
    }
}
