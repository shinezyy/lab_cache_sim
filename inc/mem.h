#ifndef __MEM_H__
#define __MEM_H__

#include <cstdint>
#include <vector>
#include <cassert>

class mem_object {
    private:
        std::vector<uint32_t> *mem;
        uint32_t mask;
        uint32_t n_columns, n_raws;

    public:
        mem_object(int n_col, int n_raw);

        uint32_t read(uint32_t addr);

        void write(uint32_t addr, uint32_t data);
};

#endif
