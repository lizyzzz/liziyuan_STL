#include "li_map.hpp"
#include <iostream>
#include "li_pair.h"

int main(int argc, char const *argv[])
{
    LI::map<int, char> m;

    m[1] = 'a';
    m[2] = 'b';
    m[4] = 'd';
    m[3] = 'c';
    LI::pair<int, char> p1(4, 'c');
    m.insert(p1);
    LI::pair<int, char> p2(10, 'f');
    m.insert(p2);
    for (auto x = m.begin(); x != m.end(); ++x) {
        std::cout << "(" << x->first << ")" << x->second << " ";
    }
    std::cout << std::endl;
    std::cout << "size: " << m.size() << std::endl;

    m.erase(2);
    m.erase(10);
    for (auto x = m.begin(); x != m.end(); ++x) {
        std::cout << "(" << x->first << ")" << x->second << " ";
    }
    std::cout << std::endl;
    std::cout << "size: " << m.size() << std::endl;

    LI::map<int, char>::iterator it;
    it = m.find(5);
    if (it != m.end()) {
        std::cout << "find "<< it->first << ": " << it->second << std::endl;
    }
    it = m.find(4);
    if (it != m.end()) {
        std::cout << "find "<< it->first << ": " << it->second << std::endl;
    }


    return 0;
}
