#include "mem.h"
#include "debug.h"
#include <cstdlib>

using namespace std;

mem_object:: mem_object(int n_col, int n_raw) {
    mask = ~((~0) << n_col);
    n_columns = n_col;
    n_raws = n_raw;
    mem = new vector<uint32_t>(n_raws);
}

uint32_t mem_object:: read(uint32_t addr) {
    assert(addr < n_raws);
    /*
    if(addr >= n_raws) {
        log_var(addr);
        log_var(n_raws);
        exit(0);
    }
    */
    uint32_t primetive_data = (*mem)[addr];
    return primetive_data & mask;
}

void mem_object:: write(uint32_t addr, uint32_t data) {
    assert(addr < n_raws);
    (*mem)[addr] = data & mask;
}
