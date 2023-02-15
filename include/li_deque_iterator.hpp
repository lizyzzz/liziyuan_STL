#ifndef LI_DEQUE_ITERATOR_H_
#define LI_DEQUE_ITERATOR_H_

#include "li_iterator.h"

// deque 容器的迭代器类型
// 用连续数组存放指向每段缓冲区的指针, 实现形式上的连续
namespace LI {

    // 如果 n 不为 0 直接返回(用户指定的)
    // 如果 n 为 0 , 则使用默认值
    //    如果 sz < 512 则 返回 512 / sz
    //    如果 sz >= 512 则 返回 1
    inline size_t __deque_buf_size(size_t n, size_t sz) {
        return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
    }

    template <class T, class Ref, class Ptr, size_t BufSiz>
    struct __deque_iterator {
        typedef __deque_iterator<T, T&, T*, BufSiz> iterator;
        typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;

        // 本来每种新的迭代器都应该继承 LI::iterator 
        // 但这个迭代器比较复杂, 所以要自己编写五种类型
        typedef random_access_iterator_tag   iterator_category;
        typedef T                            value_type;
        typedef Ptr                          pointer;
        typedef Ref                          reference;
        typedef size_t                       size_type;
        typedef ptrdiff_t                    difference_type;

        typedef T**  map_pointer; // 指向管控中心的类型

        typedef __deque_iterator self;

        static size_t buffer_size() {
            return __deque_buf_size(BufSiz, sizeof(T));
        }

        T* cur; // 此迭代器所指之缓冲区中的当前元素
        T* first; // 此迭代器所指之缓冲区中的第一个元素
        T* last; // 此迭代器所指之缓冲区中的最后一个元素的下一位置
        map_pointer node; // 指向管控中心

        // 以下为 deque 迭代器的几个关键行为

        // 一旦行进时遇到缓冲区的边缘, 要特别小心, 视前进或后退而定
        // 可能需要调用 set_node() 跳到下一个缓冲区
        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + difference_type(buffer_size());
        }

        // 重载运算子

        reference operator*() {
            return *cur;
        }
        pointer operator->() {
            return &(operator*());
        }

        difference_type operator-(const self& x) const {
            return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
        }
        // 前置递增
        self& operator++() {
            ++cur;
            // 当前为最后一个元素
            if (cur == last) {
                set_node(node + 1); // 切换到下一个缓冲区
                cur = first;  // 的第一个元素
            }
            return *this;
        }

        // 后置递增
        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        // 前置递减
        self& operator--() {
            // cur 为缓冲区第一个元素
            if (cur == first) {
                set_node(node - 1); // 切换到前一个缓冲区
                cur = last; // 的最后一个元素 的下一位置
            }
            --cur;
            return *this;
        }
        // 后置递减
        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }

        // 以下实现随机存取, 迭代器可以直接跳跃 n 个距离 (n 可能时负值)
        self& operator+=(difference_type n) {
            difference_type offset = n + (cur - first);
            if (offset >= 0 && offset < difference_type(buffer_size())) {
                // 目标位置在同一个缓冲区中
                cur += n;
            }
            else {
                // 目标位置不在同一个缓冲区中
                difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size())    // > 0 时 正向行进
                                                        : -difference_type( (-offset - 1) / buffer_size() ) - 1; // < 0 时 反向推进
                                                        // 最后 - 1 是因为 当第一项为 0 时, 也要往后行进 1.
                set_node(node + node_offset); // 切换到正确的缓冲区
                cur = first + (offset - node_offset * difference_type(buffer_size())); // 切换到正确的元素 反向也正确
            }
            return *this;
        }
        
        self operator+(difference_type n) const {
            self tmp = *this;
            return tmp += n; // 调用 operator+=
        }

        self& operator-=(difference_type n) {
            return *this += -n; // 调用 operator+=
        }

        self operator-(difference_type n) const {
            self tmp = *this;
            return tmp -= n;
        }

        // 随机存取
        reference operator[](difference_type n) const {
            return *(*this + n); 
        }
        // 关系运算符
        bool operator==(const self& x) const {
            return cur == x.cur;
        }
        bool operator!=(const self& x) const {
            return !(*this == x);
        }
        bool operator<(const self& x) const {
            return (node == x.node) ? (cur < x.cur) : (node < x.node);
        }

    };


}

#endif