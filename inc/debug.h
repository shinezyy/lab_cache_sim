#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <cstdio>

#define log(format,...)    \
    do {\
        printf("LOG:\t" format "\tin %s,function %s, line: %d\n",\
        ##__VA_ARGS__,__FILE__,__func__,__LINE__);\
    } while(0)

#define log_var(x) \
    do {\
        log("variable " #x " is 0x%x", x);\
    }while(0)

#endif
