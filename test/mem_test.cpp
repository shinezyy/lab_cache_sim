#include "mem.h"

#include <iostream>
using namespace std;

int main() {
    mem_object* m = new mem_object(1, 10);
    m->write(4, 21);
    cout << m->read(4) << endl;
    return 0;
}
