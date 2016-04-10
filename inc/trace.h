#ifndef __TRACE_H__
#define __TRACE_H__

#include <vector>
#include <string>

extern std :: vector<std :: string> trace_files;
std :: vector<char *> *get_trace(std :: string& trace_file_name);
std :: vector<int> *str_to_addr(char* str);

#endif
