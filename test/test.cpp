#include <iostream>
#include <vector>
#include "cache.h"

using namespace std;

void test_direct_map_cache_mem() {
    cache_direct_map *dc = new cache_direct_map(10, 512, 0);
    dc->invalidate_all();
    dc->write(0xfff, 78);
    dc->write(0x1000, 98);
    cout << dc->match(0x3ff, 78) << endl;
    cout << dc->match(0xfff, 78) << endl;
    cout << dc->match(0x0, 98) << endl;
    cout << dc->match(0x1000, 98) << endl;
}

void test_cache() {
    cache *c1 = new cache(64 << 10, 8, 1, false);
    cout << "X" <<endl;
    cout << "read addr: 0x7f7f7f7f: " << c1->read(0x7f7f7f7f) << endl;
    cout << "write addr: 0x7f7f7f7f: " << c1->write(0x7f7f7f7f, false, nullptr) << endl;

    cout << "read addr: 0x7f7f7f7d: " << c1->read(0x7f7f7f7d) << endl;
    cout << "read addr: 0x7f7f7f7e: " << c1->read(0x7f7f7f7e) << endl;
    cout << "read addr: 0x7f7f7f7f: " << c1->read(0x7f7f7f7f) << endl;
    cout << "read addr: 0x7f7f7f80: " << c1->read(0x7f7f7f80) << endl;

    cache *c2 = new cache(32 << 10, 32, 4, true);
    cout << "read addr: 0x7f7f7f7f: " << c2->read(0x7f7f7f7f) << endl;
    cout << "write addr: 0x7f7f7f7f: " << c2->write(0x7f7f7f7f, false, nullptr) << endl;

    cout << "read addr: 0x7f7f7f7d: " << c2->read(0x7f7f7f7d) << endl;
    cout << "read addr: 0x7f7f7f7e: " << c2->read(0x7f7f7f7e) << endl;
    cout << "read addr: 0x7f7f7f7f: " << c2->read(0x7f7f7f7f) << endl;
    cout << "read addr: 0x7f7f7f5f: " << c2->read(0x7f7f7f5f) << endl;
    cout << "read addr: 0x7f7f7f60: " << c2->read(0x7f7f7f60) << endl;
}

extern vector<string> trace_files;
extern vector<char *> *get_trace(string& trace_file_name);

void test_fin() {
    vector<char *> *v_trace = get_trace(trace_files[0]);
    unsigned int i;
    for(i = 0; i < v_trace->size(); i++) {
        cout << (*v_trace)[i];
    }
}

extern void test_L1();
extern void test_L1_L2();
extern void test_L1_L2_vict();

int main() {
    //test_L1();
    //test_L1_L2();
    test_L1_L2_vict();
    //test_fin();
    //test_cache();
    //test_direct_map_cache_mem();
    return 0;
}
