#include <iostream>
#include "li_vector.hpp"

class Int {
public:
    Int() : m_i(0) { }
    explicit Int(int i) : m_i(i) { }
    friend std::ostream& operator<<(std::ostream& out, const Int& x);
private:
    int m_i;
};

std::ostream& operator<<(std::ostream& out, const Int& x) {
    out << x.m_i;
    return out;
}


int main(int argc, char const *argv[])
{
    // std::cout << "hello world" << std::endl;

    // LI::vector<int> v(2, 1);
    // v.push_back(1);
    // v.push_back(2);
    // v.push_back(3);
    // v.push_back(4);
    // std::cout << "size : " << v.size() << std::endl;
    // std::cout << "capacity : " << v.capacity() << std::endl;

    // for (int i = 0; i < v.size(); ++i) {
    //     std::cout << v[i] << " ";
    // }
    // std::cout << std::endl;

    // v.pop_back();
    // for (int i = 0; i < v.size(); ++i) {
    //     std::cout << v[i] << " ";
    // }
    // std::cout << std::endl;

    // v.insert(v.begin(), 4, 5);
    // for (int i = 0; i < v.size(); ++i) {
    //     std::cout << v[i] << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "size : " << v.size() << std::endl;
    // std::cout << "capacity : " << v.capacity() << std::endl;

    // v.erase(v.begin(), v.begin() + 2);
    // for (int i = 0; i < v.size(); ++i) {
    //     std::cout << v[i] << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "size : " << v.size() << std::endl;
    // std::cout << "capacity : " << v.capacity() << std::endl;

    // v.reserve(50);
    // for (int i = 0; i < v.size(); ++i) {
    //     std::cout << v[i] << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "size : " << v.size() << std::endl;
    // std::cout << "capacity : " << v.capacity() << std::endl;

    // v.resize(60, 2);
    // for (int i = 0; i < v.size(); ++i) {
    //     std::cout << v[i] << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "size : " << v.size() << std::endl;
    // std::cout << "capacity : " << v.capacity() << std::endl;

    // v.resize(10);
    // for (int i = 0; i < v.size(); ++i) {
    //     std::cout << v[i] << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "size : " << v.size() << std::endl;
    // std::cout << "capacity : " << v.capacity() << std::endl;

    LI::vector<Int> v(2, Int(1));
    v.push_back(Int(1));
    v.push_back(Int(2));
    v.push_back(Int(3));
    v.push_back(Int(4));
    std::cout << "size : " << v.size() << std::endl;
    std::cout << "capacity : " << v.capacity() << std::endl;

    for (int i = 0; i < v.size(); ++i) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;

    v.pop_back();
    for (int i = 0; i < v.size(); ++i) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;

    v.insert(v.begin(), 4, Int(5));
    for (int i = 0; i < v.size(); ++i) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size : " << v.size() << std::endl;
    std::cout << "capacity : " << v.capacity() << std::endl;

    v.erase(v.begin(), v.begin() + 2);
    for (int i = 0; i < v.size(); ++i) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size : " << v.size() << std::endl;
    std::cout << "capacity : " << v.capacity() << std::endl;

    v.reserve(50);
    for (int i = 0; i < v.size(); ++i) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size : " << v.size() << std::endl;
    std::cout << "capacity : " << v.capacity() << std::endl;

    v.resize(60, Int(2));
    for (int i = 0; i < v.size(); ++i) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size : " << v.size() << std::endl;
    std::cout << "capacity : " << v.capacity() << std::endl;

    v.resize(10);
    for (int i = 0; i < v.size(); ++i) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "size : " << v.size() << std::endl;
    std::cout << "capacity : " << v.capacity() << std::endl;



    return 0;
}
