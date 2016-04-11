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

// inherent from cache:
// override read and write

class victim_cache : public cache {
    protected:
        void invalidate_a_line(uint32_t addr); // this line must be in victim cache
    public:
        bool write(uint32_t addr, bool cmp, uint32_t *victim);
        bool read(uint32_t addr);
};

bool victim_cache :: write(uint32_t addr, bool cmp, uint32_t *victim) {
    assert(cmp == false);
    return cache::write(addr, cmp, victim);
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

void victim_cache :: invalidate_a_line(uint32_t addr) {
}

#define swap_line() \
    do {\
        c1->write(addr, false, &victim_addr_from_l1);\
        if(victim_addr_from_l1) { \
            vc->write(victim_addr_from_l1, false, nullptr);\
        }\
    }while(0)

uint64_t benchmark_L1_L2_vict(cache *c1, cache *vc, cache *c2, 
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
                l1_miss = true;
            }
        }
        else { // store 
            if(!c1->write(addr, true, nullptr)) { // miss
                l1_miss = true;
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
            // swap two lines between victim cache and L1 cache
            // In reality, during a write cycle, we need one more cycle to write to 
            // newly loaded line in L1 cache, which was emitted here
            swap_line();
            continue;
        }

        // victim cache miss :
        // L2 cache :
        all_cycles += L2_LTC;
        bool l2_miss = false;

        if(load) {
            if(!c2->read(addr)) { // miss
                l2_miss = true;
            }
        }
        else { // store
            if(!c2->write(addr, true, nullptr)) { // miss
                l2_miss = true;
            }
        }

        if(!l2_miss) {
            swap_line();
            continue;
        }

        // L2 miss :
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
    cache *vc = new cache(1 << 10, 32, -1, true); 
    cache *c2 = new cache(2 << 20, 128, 8, true); 
    for(i = 0; i < trace_files.size(); i++) {
        c1->invalidate_all();
        vc->invalidate_all();
        c2->invalidate_all();
        cout << "cycles of " << trace_files[i] << ":\n";
        cout << benchmark_L1_L2_vict(c1, vc, c2, get_trace(trace_files[i])) << endl;
    }
}
