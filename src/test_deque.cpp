#include "li_deque.hpp"
#include <iostream>


int main(int argc, char const *argv[])
{
    std::cout << "hello world" << std::endl;

    LI::deque<int> d;
    d.push_back(1);
    d.push_back(2);
    d.push_front(3);
    d.push_front(4);

    for (int i = 0; i < d.size(); ++i) {
        std::cout << d[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size: " << d.size() << std::endl;  
    
    d.pop_back();
    d.pop_front();
    for (int i = 0; i < d.size(); ++i) {
        std::cout << d[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size: " << d.size() << std::endl;
    d.push_back(5);
    d.push_back(5);
    for (int i = 0; i < d.size(); ++i) {
        std::cout << d[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size: " << d.size() << std::endl;

    LI::deque<int>::iterator iter = d.begin();
    iter += 2;
    iter = d.insert(iter, 10);
    for (int i = 0; i < d.size(); ++i) {
        std::cout << d[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size: " << d.size() << std::endl;

    iter = d.erase(iter);
    for (int i = 0; i < d.size(); ++i) {
        std::cout << d[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size: " << d.size() << std::endl;

    d.erase(iter, iter + 2);
    for (int i = 0; i < d.size(); ++i) {
        std::cout << d[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size: " << d.size() << std::endl;

    d.clear();
    std::cout << "size: " << d.size() << std::endl;


    return 0;
}
