#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <cstdint>

#include "cache.h"

using namespace std;

/* trace file input */

string file_dir = "./traces/";

vector<string> trace_files = {
    "gcc.trace",
    "gzip.trace",
    "mcf.trace",
    "swim.trace",
    "twolf.trace"
};

vector<char *> *get_trace(string& trace_file_name) {
    ifstream infile;
    infile.open(file_dir + trace_file_name);
    vector<char *> *ret = new vector<char *>;
    while(!infile.eof()) {
        char *str = new char[20];
        infile.getline(str, 20);
        ret->push_back(str);
    }
    return ret;
}


/* Latency */

const uint32_t L1_LTC = 1;
const uint32_t VIC_LTC = 1;     // victim cache
const uint32_t L2_LTC = 10;
const uint32_t OC_LTC = 100;    // off-chip

uint64_t benchmark_L1(cache *c, vector<char *> *v_trace) { // return cycles
    uint32_t i;
    uint64_t all_cycles = 0;
    for(i = 0; i < v_trace->size(); i++) {
        bool load = true;
        uint32_t addr, n_cycles;
        if((*v_trace)[i][0] == 's') {
            load = false;
        }

        sscanf((*v_trace)[i] + 2, "0x%x %d", &addr, &n_cycles);
        all_cycles += n_cycles;

        if(load) {
            all_cycles += L1_LTC;
            if(!c->read(addr)) { // miss
                all_cycles += OC_LTC;
                c->write(addr, false, nullptr); // load block
            }
        }
        else { // store 
            all_cycles += L1_LTC;
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


    cout << "Testing c1:\n";
    cache *c1 = new cache(64 << 10, 8, 1, false); 
    for(i = 0; i < trace_files.size(); i++) {
        c1->invalidate_all();
        cout << "cycles of " << trace_files[i] << ":\n";
        cout << benchmark_L1(c1, get_trace(trace_files[i])) << endl;
    }

    cout << "Testing c2:\n";
    cache *c2 = new cache(32 << 10, 32, 4, true); 
    for(i = 0; i < trace_files.size(); i++) {
        c2->invalidate_all();
        cout << "cycles of " << trace_files[i] << ":\n";
        cout << benchmark_L1(c2, get_trace(trace_files[i])) << endl;
    }

    cout << "Testing c3:\n";
    cache *c3 = new cache(8 << 10, 64, -1, false); 
    for(i = 0; i < trace_files.size(); i++) {
        c3->invalidate_all();
        cout << "cycles of " << trace_files[i] << ":\n";
        cout << benchmark_L1(c3, get_trace(trace_files[i])) << endl;
    }
}


