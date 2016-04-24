#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cassert>

#include "debug.h"
#include "cache.h"
#include "trace.h"
#include "latency.h"

using namespace std;

bool verbose = false;

static uint64_t l1_r_hit, l1_w_hit, l1_r_miss, l1_w_miss;
static uint64_t l2_r_hit, l2_w_hit, l2_r_miss, l2_w_miss;
static uint64_t mem_r, mem_w;
static uint64_t l1_r, l1_w;
static uint64_t l2_r, l2_w;

static void counter_init(){
    l1_r_hit = 0;
    l1_w_hit = 0;
    l1_r_miss = 0;
    l1_w_miss = 0;
    l2_r_hit = 0;
    l2_w_hit = 0;
    l2_r_miss = 0;
    l2_w_miss = 0;
    mem_r = 0;
    mem_w = 0;
    l1_r = 0;
    l1_w = 0;
    l2_r = 0;
    l2_w = 0;
}

int print_count = 0;

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
        if(load) {
            l1_r += 1;
            if(!c1->read(addr)) { // miss
                l1_r_miss += 1;
            }
            else {
                l1_r_hit += 1;
                continue;
            }
        }
        else { // store 
            l1_w += 1;
            if(!c1->write(addr, true, nullptr)) { // miss
                l1_w_miss += 1;
            }
            else {
                l1_w_hit += 1;
                continue;
            }
        }

        // L1 miss :
        // L2 cache :
        all_cycles += L2_LTC;
        bool l2_miss = false;
        if(load) {
            l2_r += 1;
            if(!c2->read(addr)) { // miss
                l2_r_miss += 1;
                l2_miss = true;
            }
            else {
                l2_r_hit += 1;
            }
        }
        else { // store 
            l2_w += 1;
            if(!c2->write(addr, true, nullptr)) { // miss
                l2_w_miss += 1;
                l2_miss = true;
            }
            else {
                l2_w_hit += 1;
            }
        }

        if(!l2_miss) {
            c1->write(addr, false, nullptr);
            continue;
        }

        // L2 miss :
        if (load) {
            mem_r += 1;
        }
        else {
            mem_w += 1;
        }
        all_cycles += OC_LTC; // fetch from mem
        c2->write(addr, false, nullptr); // load block
        c1->write(addr, false, nullptr); // inclusive design
        assert(c1->read(addr));
    }
    return all_cycles;
}

void test_L1_L2() {
    unsigned int i;

    cout << "========================================\n";
    cout << "Testing L1 + L2:\n";
    cache *c1 = new cache(32 << 10, 32, 4, true); 
    cache *c2 = new cache(2 << 20, 128, 8, true); 
    ofstream ofs;
    ofs.open("L1_L2.csv", ios::out);
    for(i = 0; i < trace_files.size(); i++) {
        counter_init();
        c1->invalidate_all();
        c2->invalidate_all();

        ofs << trace_files[i] << endl;
        ofs << "cycles: " << benchmark_L1_L2(c1, c2, get_trace(trace_files[i])) << endl;

        ofs << ",number of accesses";
        ofs << ",number of loads";
        ofs << ",number of stores";
        ofs << ",hit rate";
        ofs << ",load hit rate";
        ofs << ",store hit rate";
        ofs << endl;

        ofs << "L1 cache,";
        ofs << l1_r + l1_w << ',';
        ofs << l1_r << ',';
        ofs << l1_w << ',';
        cout << "L1 hit: " << l1_r_hit + l1_w_hit << endl;
        ofs << ((double)(l1_r_hit + l1_w_hit))/(l1_r + l1_w) << ',';
        ofs << ((double)l1_r_hit)/l1_r << ',';
        ofs << ((double)l1_w_hit)/l1_w;
        ofs << endl;

        ofs << "L2 cache,";
        ofs << l2_r + l2_w << ',';
        ofs << l2_r << ',';
        ofs << l2_w << ',';
        cout << "L2 hit: " << l2_r_hit + l2_w_hit << endl;
        ofs << ((double)(l2_r_hit + l2_w_hit))/(l2_r + l2_w) << ',';
        ofs << ((double)l2_r_hit)/l2_r << ',';
        ofs << ((double)l2_w_hit)/l2_w;
        ofs << endl;

        ofs << "memory,";
        ofs << mem_r + mem_w << ',';
        ofs << mem_r << ',';
        ofs << mem_w << ",,,";
        ofs << endl;
    }
    ofs.close();
}

