#include "li_rbtree.hpp"
#include "li_functional.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    LI::rb_tree<int, int, LI::identity<int>, LI::less<int>> itree;
    std::cout << itree.size() << std::endl;

    itree.insert_unique(10);
    itree.insert_unique(7);
    itree.insert_unique(8);

    std::cout << itree.size() << std::endl;
    for (auto x = itree.begin(); x != itree.end(); ++x) {
        std::cout << *x << "(" << x.node->color << ")" << " ";
    }
    std::cout << std::endl;

    itree.insert_unique(15);
    itree.insert_unique(5);
    itree.insert_unique(6);
    itree.insert_unique(11);
    itree.insert_unique(13);
    itree.insert_unique(12);

    std::cout << itree.size() << std::endl;
    for (auto x = itree.begin(); x != itree.end(); ++x) {
        std::cout << *x << "(" << x.node->color << ")" << " ";
    }
    std::cout << std::endl;

    itree.erase(10);
    std::cout << itree.size() << std::endl;
    for (auto x = itree.begin(); x != itree.end(); ++x) {
        std::cout << *x << "(" << x.node->color << ")" << " ";
    }
    std::cout << std::endl;
    

    return 0;
}
