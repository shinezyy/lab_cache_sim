#include <iostream>
#include <fstream>
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

static uint64_t l1_r_hit, l1_w_hit, l1_r_miss, l1_w_miss;
static uint64_t l1_r, l1_w;
static uint64_t mem_r, mem_w;

static void counter_init(){
    l1_r_hit = 0;
    l1_w_hit = 0;
    l1_r_miss = 0;
    l1_w_miss = 0;
    mem_r = 0;
    mem_w = 0;
    l1_r = 0;
    l1_w = 0;
}


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
            l1_r += 1;
            if(!c->read(addr)) { // miss
                l1_r_miss += 1;
                all_cycles += OC_LTC;
                mem_r += 1;
                c->write(addr, false, nullptr); // load block
            }
            else {
                l1_r_hit += 1;
            }
        }
        else { // store 
            l1_w += 1;
            if(!c->write(addr, true, nullptr)) { // miss
                l1_w_miss += 1;
                all_cycles += OC_LTC;
                mem_w += 1;
                c->write(addr, false, nullptr); // load block
            }
            else {
                l1_w_hit += 1;
            }
        }
    }
    return all_cycles;
}

void test_L1() {
    unsigned int i;

    ofstream ofs;
    ofs.open("L1.csv", ios::out);
    ofs << "trace";
    ofs << ",cycles";
    ofs << ",storage";
    ofs << ",number of accesses";
    ofs << ",number of loads";
    ofs << ",number of stores";
    ofs << ",hit rate";
    ofs << ",load hit rate";
    ofs << ",store hit rate";
    ofs << endl;


    cout << "========================================\n";

    cout << "Testing L1 c1:\n";
    cache *c1 = new cache(64 << 10, 8, 1, false); 
    for(i = 0; i < trace_files.size(); i++) {
        counter_init();
        c1->invalidate_all();

        ofs << trace_files[i] << ',';
        ofs << benchmark_L1(c1, get_trace(trace_files[i])) << ',';
        ofs << "L1 cache,";

        ofs << l1_r + l1_w << ',';
        ofs << l1_r << ',';
        ofs << l1_w << ',';
        cout << "L1 hit: " << l1_r_hit + l1_w_hit << endl;
        ofs << ((double)(l1_r_hit + l1_w_hit))/(l1_r + l1_w) << ',';
        ofs << ((double)l1_r_hit)/l1_r << ',';
        ofs << ((double)l1_w_hit)/l1_w;
        ofs << endl;

        ofs << ",,memory," << mem_r + mem_w << ',';
        ofs << mem_r << ',';
        ofs << mem_w << ",,,";
        ofs << endl;
    }

    cout << "Testing L1 c2:\n";
    cache *c2 = new cache(32 << 10, 32, 4, true); 
    for(i = 0; i < trace_files.size(); i++) {
        counter_init();
        c2->invalidate_all();

        ofs << trace_files[i] << ',';
        ofs << benchmark_L1(c2, get_trace(trace_files[i])) << ',';
        ofs << "L1 cache,";

        ofs << l1_r + l1_w << ',';
        ofs << l1_r << ',';
        ofs << l1_w << ',';
        cout << "L1 hit: " << l1_r_hit + l1_w_hit << endl;
        ofs << ((double)(l1_r_hit + l1_w_hit))/(l1_r + l1_w) << ',';
        ofs << ((double)l1_r_hit)/l1_r << ',';
        ofs << ((double)l1_w_hit)/l1_w;
        ofs << endl;

        ofs << ",,memory," << mem_r + mem_w << ',';
        ofs << mem_r << ',';
        ofs << mem_w << ",,,";
        ofs << endl;
    }

    cout << "Testing L1 c3:\n";
    cache *c3 = new cache(8 << 10, 64, -1, false); 
    for(i = 0; i < trace_files.size(); i++) {
        counter_init();
        c3->invalidate_all();

        ofs << trace_files[i] << ',';
        ofs << benchmark_L1(c3, get_trace(trace_files[i])) << ',';
        ofs << "L1 cache,";

        ofs << l1_r + l1_w << ',';
        ofs << l1_r << ',';
        ofs << l1_w << ',';
        cout << "L1 hit: " << l1_r_hit + l1_w_hit << endl;
        ofs << ((double)(l1_r_hit + l1_w_hit))/(l1_r + l1_w) << ',';
        ofs << ((double)l1_r_hit)/l1_r << ',';
        ofs << ((double)l1_w_hit)/l1_w;
        ofs << endl;

        ofs << ",,memory," << mem_r + mem_w << ',';
        ofs << mem_r << ',';
        ofs << mem_w << ",,,";
        ofs << endl;
    }
    ofs.close();
}
