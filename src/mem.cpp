#include "mem.h"
#include "debug.h"
#include <cstdlib>

using namespace std;

mem_object:: mem_object(int n_col, int n_row) {
    mask = ~((~0) << n_col);
    n_columns = n_col;
    n_rows = n_row;
    mem = new vector<uint32_t>(n_rows);
}

uint32_t mem_object:: read(uint32_t addr) {
    assert(addr < n_rows);
    /*
    if(addr >= n_rows) {
        log_var(addr);
        log_var(n_rows);
        exit(0);
    }
    */
    uint32_t primetive_data = (*mem)[addr];
    return primetive_data & mask;
}

void mem_object:: write(uint32_t addr, uint32_t data) {
    assert(addr < n_rows);
    (*mem)[addr] = data & mask;
}
