#include <cstdlib>
#include "debug.h"
#include "cache.h"

using namespace std;

extern bool verbose;

static uint32_t log2(uint32_t x) {
    if(x != 1) {
        return 1 + log2(x >> 1);
    }
    else {
        return 0;
    }
}

// count in byte
cache :: cache(uint32_t size, uint32_t line_size, int32_t assoc, bool en_lru, bool is_v) {
    uint32_t age_width, n_rows_all, n_ways, n_rows_in_way;
    //uint32_t offset_width, index_width, tag_width;
    is_victim_cache = is_v;
    if(en_lru) { 
        age_width = LRU_LEN;
        lru = true;
    }
    else {
        age_width = 0;
        lru = false;
    }

    n_rows_all = size/line_size;

    if(assoc == -1) { //fully
        n_ways = n_rows_all;
    }
    else {
        n_ways = assoc;
    }
    n_rows_in_way = n_rows_all / n_ways; // 1 for fully assoc...

    index_width = log2(n_rows_in_way);
    offset_width = log2(line_size);
    tag_width = BUS_WIDTH - index_width - offset_width;

    // init members
    rand_sel = 0;
    uint32_t i;
    for(i = 0; i < n_ways; i++) {
        simple_cache.push_back(new cache_direct_map(tag_width, n_rows_in_way, age_width));
        simple_cache[i]->invalidate_all();
    }
}

bool cache :: write(uint32_t addr, bool cmp, uint32_t *victim) {
    if(cmp) {
        for(auto it = simple_cache.begin(); it != simple_cache.end(); it++) {
            (*it)->time_pass_by(addr_to_index(addr));
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
        uint32_t victim_idx = ~0;
        uint32_t n_ways = simple_cache.size();
        uint32_t i;
        bool already_exist = false; // the expected line is already in the cache
                                    // which is only allowed in victim cache
        for(i = 0; i < n_ways; i++) {
            if(simple_cache[i]->match_without_aging(addr_to_tag(addr),addr_to_index(addr))) {
                already_exist = true;
            }
        }
        if (already_exist) {
            if(!is_victim_cache) {
                print_cond("addr: 0x%x\n", addr);
                exit(0);
            }
            return true;
        }
        // find invalid line :
        for(i = 0; i < n_ways; i++) {
            if(!simple_cache[i]->get_valid(addr_to_index(addr))) {
                victim_idx = i;
                break;
            }
        }

        //print_cond("start looking for lru-----------------------------------\n");
        if(i == n_ways) { // invalid line not found :
            if(lru) {
                uint32_t min = simple_cache[0]->get_recent_use(addr_to_index(addr));
                uint32_t min_idx = 0;
                for(i = 1; i < n_ways; i++) {
                    //print_cond("RU: %d\n", simple_cache[i]->get_recent_use(addr_to_index(addr))); 
                    if(simple_cache[i]->get_recent_use(addr_to_index(addr)) < min) {
                        min = simple_cache[i]->get_recent_use(addr_to_index(addr));
                        min_idx = i;
                    }
                }
                victim_idx = min_idx;
            }
            else {
                rand_sel += 1;
                if(rand_sel >= n_ways){
                    rand_sel -= n_ways;
                }
                victim_idx = rand_sel;
            }
        }
        else {
            print_cond("found inv line\n");
        }
        print_cond("victim_idx: 0x%x\n\n",victim_idx);
        //print_cond("found lru-----------------------------------\n");

        print_cond("----tags before replacement----\n");
        for(i = 0; i < n_ways; i++) {
            print_cond("0x%x\n", simple_cache[i]->get_tag(addr_to_index(addr)));
        }
        // do replacement
        uint32_t victim_valid = simple_cache[victim_idx]->get_valid(addr_to_index(addr));
        if(victim != nullptr) { // set victim to nullptr to indicate it is not requsted
            if(!victim_valid) {
                print_cond("victim invalid\n");
                *victim = 0;
            }
            else {
                uint32_t victim_tag = simple_cache[victim_idx]->get_tag(addr_to_index(addr));
                *victim = (victim_tag << (index_width + offset_width))
                    | (addr_to_index(addr) << offset_width);
            }
        }
        simple_cache[victim_idx]->write(addr_to_tag(addr), addr_to_index(addr));
        print_cond("tag new: 0x%x\n", addr_to_tag(addr));
        print_cond("index: 0x%x\n", addr_to_index(addr));
        print_cond("----tags after replacement----\n");
        for(i = 0; i < n_ways; i++) {
            print_cond("0x%x\n", simple_cache[i]->get_tag(addr_to_index(addr)));
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
        (*it)->time_pass_by(addr_to_index(addr));
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
    uint32_t n_ways = simple_cache.size();
    for(i = 0; i < n_ways; i++) {
        simple_cache[i]->invalidate_all();
    }
}

void cache :: invalidate_a_line(uint32_t addr) {
    uint32_t i;
    uint32_t n_ways = simple_cache.size();
    uint32_t index = addr_to_index(addr);
    assert(index == 0);
    uint32_t tag = addr_to_tag(addr);

    for(i = 0; i < n_ways; i++) {
        if(simple_cache[i]->match_without_aging(tag, index)) {
            simple_cache[i]->invalidate_a_line(index);
            print_cond("invalidated tag: 0x%x", addr_to_tag(addr));
            return;
        }
    }
    assert(0);
}
