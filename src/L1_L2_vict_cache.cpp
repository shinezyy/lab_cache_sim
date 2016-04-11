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

uint64_t benchmark_L1_L2_vict(cache *c1, cache *vc, cache *c2, 
        vector<char *> *v_trace) { // return cycles
    uint32_t i;
    uint64_t all_cycles = 0;
    uint32_t trace_size = v_trace->size();
    for(i = 0; i < trace_size; i++) {
        uint32_t load, addr, n_cycles;
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
        if(load) {
            if(!vc->read(addr)) { // miss
                vc_miss = true;
            }
        }
        else { // store 
            if(!vc->write(addr, true, nullptr)) { // miss
                vc_miss = true;
            }
        }

        if(!vc_miss) {
            // swap one line in L1 cache and victim cache
            uint32_t victim_addr;
            c->write(addr, false, &victim_addr);
            if(victim_addr) { // 0 for invalid
                vc->write(victim_addr, false, 
            continue;
        }

    }
    return all_cycles;
}

void test_L1_L2_vict() {
    unsigned int i;


    cout << "Testing c1:\n";
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
