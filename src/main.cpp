#include "../include/data_structures/python_list.h"
#include <iostream>
#include <string>
#include "../include/data_structures/vector.h"

int main() {
    algolib::python_list l;
    using type = algolib::vector<int>;
    l.push_back("zalata");
    l.push_back(17);
    l.push_back(type({4, 2006}));
    std::cout << l[0] << ' ' << l[1] << ' '
        << l[2] << ' ' << l[2].as<type>()[0]
        << ' ' << l[2].as<type>()[1] << '\n';
}