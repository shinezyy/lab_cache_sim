#include <fstream>
#include <string>
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
        ret->push_back(str);
    }
    return ret;
}


