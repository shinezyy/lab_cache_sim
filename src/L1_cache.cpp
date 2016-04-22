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

uint64_t benchmark_L1(cache *c, vector<char *> *v_trace) { // return cycles
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
        all_cycles += L1_LTC;

        if(load) {
            if(!c->read(addr)) { // miss
                all_cycles += OC_LTC;
                c->write(addr, false, nullptr); // load block
            }
        }
        else { // store 
            if(!c->write(addr, true, nullptr)) { // miss
                all_cycles += OC_LTC;
                c->write(addr, false, nullptr); // load block
            }
        }
    }
    return all_cycles;
}

void test_L1() {
    unsigned int i;

    cout << "========================================\n";
    cout << "Testing L1 c1:\n";
    cache *c1 = new cache(64 << 10, 8, 1, false); 
    for(i = 0; i < trace_files.size(); i++) {
        c1->invalidate_all();
        cout << "cycles of " << trace_files[i] << ":\n";
        cout << benchmark_L1(c1, get_trace(trace_files[i])) << endl;
    }

    cout << "Testing L1 c2:\n";
    cache *c2 = new cache(32 << 10, 32, 4, true); 
    for(i = 0; i < trace_files.size(); i++) {
        c2->invalidate_all();
        cout << "cycles of " << trace_files[i] << ":\n";
        cout << benchmark_L1(c2, get_trace(trace_files[i])) << endl;
    }

    cout << "Testing L1 c3:\n";
    cache *c3 = new cache(8 << 10, 64, -1, false); 
    for(i = 0; i < trace_files.size(); i++) {
        c3->invalidate_all();
        cout << "cycles of " << trace_files[i] << ":\n";
        cout << benchmark_L1(c3, get_trace(trace_files[i])) << endl;
    }
}


