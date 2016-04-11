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

uint64_t benchmark_L1_L2(cache *c1, cache *c2, vector<char *> *v_trace) { // return cycles
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
            c1->write(addr, false, nullptr);
            continue;
        }

        // L2 miss :
        all_cycles += OC_LTC; // fetch from mem
        c2->write(addr, false, nullptr); // load block
        c1->write(addr, false, nullptr); // inclusive design
    }
    return all_cycles;
}

void test_L1_L2() {
    unsigned int i;

    cout << "Testing L1 + L2:\n";
    cache *c1 = new cache(32 << 10, 32, 4, true); 
    cache *c2 = new cache(2 << 20, 128, 8, true); 
    for(i = 0; i < trace_files.size(); i++) {
        c1->invalidate_all();
        c2->invalidate_all();
        cout << "cycles of " << trace_files[i] << ":\n";
        cout << benchmark_L1_L2(c1, c2, get_trace(trace_files[i])) << endl;
    }
}

