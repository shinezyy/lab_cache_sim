#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <cstdint>
#include <cstdio>

#include "debug.h"
#include "cache.h"
#include "trace.h"
#include "latency.h"

using namespace std;

extern bool verbose;

// inherent from cache:
// override read and write

class victim_cache : public cache {
    public:
        victim_cache(uint32_t size, uint32_t line_size, uint32_t assoc, bool en_lru):
            cache(size, line_size, assoc, en_lru, true) {}
        bool write(uint32_t addr, bool cmp, uint32_t *victim);
        bool read(uint32_t addr);
};

bool victim_cache :: write(uint32_t addr, bool cmp, uint32_t *victim) {
    assert(cmp == false);
    cache :: write(addr, cmp, victim);
    assert(cache :: read(addr));
    return true;
}

bool victim_cache :: read(uint32_t addr) {
    if(cache::read(addr)){ // hit, invalidate it 
        invalidate_a_line(addr);
        return true;
    }
    else {
        return false;
    }
}

static uint64_t l1_r_hit, l1_w_hit, l1_r_miss, l1_w_miss;
static uint64_t l2_r_hit, l2_w_hit, l2_r_miss, l2_w_miss;
static uint64_t n_vc_hit, n_vc_miss;
static uint64_t mem_r, mem_w;

static void counter_init(){
    l1_r_hit = 0;
    l1_w_hit = 0;
    l1_r_miss = 0;
    l1_w_miss = 0;
    l2_r_hit = 0;
    l2_w_hit = 0;
    l2_r_miss = 0;
    l2_w_miss = 0;
    n_vc_hit = 0;
    n_vc_miss = 0;
    mem_r = 0;
    mem_w = 0;
}

#define swap_line() \
    do {\
        print_cond("---- L1 cache begin ----\n");\
        c1->write(addr, false, &victim_addr_from_l1);\
        print_cond("---- L1 cache end ----\n\n");\
        if(victim_addr_from_l1) { \
            print_cond("---- victim cache begin ----\n");\
            vc->write(victim_addr_from_l1, false, nullptr);\
            print_cond("---- victim cache end ----\n\n");\
        }\
    }while(0)

uint64_t benchmark_L1_L2_vict(cache *c1, victim_cache *vc, cache *c2, 
        vector<char *> *v_trace) { // return cycles
    uint32_t i;
    uint64_t all_cycles = 0;
    uint32_t trace_size = v_trace->size();
    for(i = 0; i < trace_size; i++) {
        uint32_t load, addr, n_cycles;
        uint32_t victim_addr_from_l1;

        vector<int> *ret = str_to_addr((*v_trace)[i]);
        // unpack return val :
        load = (*ret)[0];
        addr = (*ret)[1];
        n_cycles = (*ret)[2];

        //printf("%d 0x%x %d\n", load, addr, n_cycles);
        all_cycles += n_cycles;

        // L1 cache :
        all_cycles += L1_LTC;
        bool l1_miss = false;
        if(load) {
            if(!c1->read(addr)) { // miss
                l1_r_miss += 1;
                l1_miss = true;
            }
            else {
                l1_r_hit += 1;
            }
        }
        else { // store 
            if(!c1->write(addr, true, nullptr)) { // miss
                l1_miss = true;
                l1_w_miss += 1;
            }
            else {
                l1_w_hit += 1;
            }
        }

        if(!l1_miss) {
            continue;
        }

        // L1 miss :
        // victim cache :
        all_cycles += VIC_LTC;
        bool vc_miss = false;

        // L1 cache read from victim cache, but not write to it directly
        if(!vc->read(addr)) { // miss
            vc_miss = true;
        }

        if(!vc_miss) {
            n_vc_hit += 1;
            // swap two lines between victim cache and L1 cache
            // In reality, during a write cycle, we need one more cycle to write to 
            // newly loaded line in L1 cache, which was omitted here
            swap_line();
            continue;
        }
        n_vc_miss += 1;

        // victim cache miss :
        // L2 cache :
        all_cycles += L2_LTC;
        bool l2_miss = false;

        if(load) {
            if(!c2->read(addr)) { // miss
                l2_r_miss += 1;
                l2_miss = true;
            }
            else {
                l2_r_hit += 1;
            }
        }
        else { // store
            if(!c2->write(addr, true, nullptr)) { // miss
                l2_miss = true;
                l2_w_miss += 1;
            }
            else {
                l2_w_hit += 1;
            }
        }

        if(!l2_miss) {
            swap_line();
            continue;
        }

        // L2 miss :
        if (load) {
            mem_r += 1;
        }
        else {
            mem_w += 1;
        }
        all_cycles += OC_LTC;
        c2->write(addr, false, nullptr);
        swap_line();
    }
    return all_cycles;
}

void test_L1_L2_vict() {
    unsigned int i;

    cout << "Testing L1 + L2 + victim cache:\n";
    cache *c1 = new cache(32 << 10, 32, 4, true); 
    victim_cache *vc = new victim_cache(1 << 10, 32, -1, true); 
    cache *c2 = new cache(2 << 20, 128, 8, true); 
    for(i = 0; i < trace_files.size(); i++) {
        counter_init();
        c1->invalidate_all();
        vc->invalidate_all();
        c2->invalidate_all();
        cout << trace_files[i] << "-------------------------------------------\n";
        cout << "cycles: " << benchmark_L1_L2_vict(c1, vc, c2, get_trace(trace_files[i])) << endl;
        cout << "L1 hit: " << l1_r_hit + l1_w_hit 
            << ", miss: " << l1_r_miss + l1_w_miss << endl;
        cout << "L2 hit: " << l2_r_hit + l2_w_hit 
            << ", miss: " << l2_r_miss + l2_w_miss << endl;
        cout << "victim cache hit: " << n_vc_hit << ", miss: " << n_vc_miss << endl;
    }
}
