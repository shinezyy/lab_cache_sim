#ifndef __DIRECT_MAPPED_H__
#define __DIRECT_MAPPED_H__

#include <cstdint>
#include "mem.h"

class cache_direct_map {
    private:
        mem_object *valid_col;
        mem_object *tag_col;
        mem_object *age_col;
        uint32_t n_rows;

    public:
        cache_direct_map(uint32_t tag_width, uint32_t n_r, uint32_t age_width);// number of rows

        bool match(uint32_t tag_in, uint32_t addr);

        bool match_without_aging(uint32_t tag_in, uint32_t addr);

        void invalidate_all();

        void write(uint32_t tag_in, uint32_t addr);

        void time_pass_by(uint32_t addr);

        uint32_t get_recent_use(uint32_t addr);

        uint32_t get_tag(uint32_t addr);

        uint32_t get_valid(uint32_t addr);

        void invalidate_a_line(uint32_t addr);
};

#endif
