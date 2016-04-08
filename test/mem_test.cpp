#include <iostream>
#include "cache.h"

using namespace std;

void test_direct_map_cache_mem() {
    cache_direct_map *dc = new cache_direct_map(10, 512, 0);
    dc->invalidata_all();
    dc->write(0xfff, 78);
    dc->write(0x1000, 98);
    cout << dc->match(0x3ff, 78) << endl;
    cout << dc->match(0xfff, 78) << endl;
    cout << dc->match(0x0, 98) << endl;
    cout << dc->match(0x1000, 98) << endl;
}

void test_cache() {
    cache *c = new cache(64 << 10, 8, 1, false);
    cout << "X" <<endl;
    cout << "read addr: 0x7f7f7f7f: " << c->read(0x7f7f7f7f) << endl;
    cout << "write addr: 0x7f7f7f7f: " << c->write(0x7f7f7f7f, true) << endl;
    cout << "read addr: 0x7f7f7f7f: " << c->read(0x7f7f7f7f) << endl;
}

int main() {
    test_cache();
    //test_direct_map_cache_mem();
    return 0;
}
