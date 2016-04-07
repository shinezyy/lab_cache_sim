#include <iostream>
#include "cache_mem.h"

using namespace std;

int test_direct_map_cache_mem() {
    cache_direct_map *dc = new cache_direct_map(10, 512, 0);
    dc->invalidata_all();
    dc->write(0xfff, 78);
    dc->write(0x1000, 98);
    cout << dc->match(0x3ff, 78) << endl;
    cout << dc->match(0xfff, 78) << endl;
    cout << dc->match(0x0, 98) << endl;
    cout << dc->match(0x1000, 98) << endl;
    return 0;
}

int main() {
    test_direct_map_cache_mem();
    return 0;
}
