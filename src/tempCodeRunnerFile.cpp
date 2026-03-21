#include "../include/data_structures/python_list.h"
#include <iostream>
#include <string>

int main() {
    algolib::python_list l;
    l.push_back(3);
    l.push_back("hello");
    std::cout << l[0] << ' ' << l[1] << '\n';
}