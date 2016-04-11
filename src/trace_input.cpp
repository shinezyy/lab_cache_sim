#include <fstream>
#include <string>
#include <cstring>
#include <vector>

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
        if(strlen(str) >= 10) {
            ret->push_back(str);
        }
    }
    return ret;
}

vector<unsigned int> *str_to_addr(char* str) {
    unsigned int load, addr, n_cycles;
    if(str[0] == 's') {
        load = 0;
    }
    else {
        load = 1;
    }

    sscanf(str + 2, "0x%x %d", &addr, &n_cycles);
    
    vector<unsigned int> *ret = new vector<unsigned int>;
    ret->push_back(load);
    ret->push_back(addr);
    ret->push_back(n_cycles);

    return ret;
}
