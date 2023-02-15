#ifndef LI_DEQUE_H_
#define LI_DEQUE_H_

#include "li_alloc.h"
#include "li_uninitialized.h"
#include "li_deque_iterator.hpp"
#include <iostream>

// deque 容器的实现
namespace LI {

    // deque 维护一个指向 map 数组(管控中心)的指针 实现形式上的连续空间

    template <class T, class Alloc = alloc, size_t BufSiz = 0>
    class deque {
    public:
        typedef T            value_type;
        typedef T&           reference;
        typedef value_type*  pointer;
        typedef ptrdiff_t    difference_type;
        typedef size_t       size_type;

        typedef __deque_iterator<T, T&, T*, BufSiz> iterator; // 迭代器

    protected:
        typedef pointer* map_pointer; // 指向 T* 的指针

    protected:
        iterator start; // 第一个节点的迭代器
        iterator finish; // 最后一个节点的下一位置的迭代器

        map_pointer map; // 指向 map, map 是连续的空间地址, 存放每个缓存区的第一个节点的指针
        size_type map_size; // map 的长度
        
    public:
        // 一些简单的功能
        iterator begin() {
            return start;
        }
        iterator end() {
            return finish;
        }

        reference operator[] (size_type n) {
            return start[difference_type(n)]; // 调用 iterator::operator[]
        }

        reference front() {
            return *start; // 调用 iterator::operator*
        }

        reference back() {
            iterator tmp = finish;
            --tmp;
            return *tmp;  // *(finish - 1) 应该也可以
        }

        size_type size() const {
            return finish - start; // 调用 iterator::operator-
        }
        size_type max_size() const {
            return size_type(-1); // 是一个最大正值, 因为 size_type 是 unsigned
        }
        bool empty() const {
            return finish == start;
        }


    
    protected:
        // 两个空间配置器
        typedef simple_alloc<value_type, Alloc> data_allocator; // 元素空间配置器
        typedef simple_alloc<pointer, Alloc> map_allocator; // map空间配置
        
        // fill_initialize() 负责 安排好 deque 的结构, 并将元素的初始值设置妥当
        void fill_initialize(size_type n, const value_type& value);
        // create_map_and_nodes() 负责安排好 deque 的结构
        void create_map_and_nodes(size_type num_elements);
        // 配置缓冲区
        pointer allocate_node();
        void deallocate_node(pointer x);
        // buffer_size()函数 返回缓冲区的大小
        size_type buffer_size() {
            return iterator::buffer_size();
        }

        // push_back() 和 push_front() 的辅助函数
        void push_back_aux(const value_type& x);
        void push_front_aux(const value_type& x);
        // pop_back() 和 pop_front() 的辅助函数
        void pop_back_aux();
        void pop_front_aux();
        // insert() 的辅助函数
        iterator insert_aux(iterator position, const value_type& x);
        // map 节点 空间不够时 重新配置一个 map
        void reserve_map_at_back (size_type nodes_to_add = 1);
        void reserve_map_at_front (size_type nodes_to_add = 1);
        void reallocate_map (size_type nodes_to_add, bool add_at_front);

    public:
        // 构造函数
        deque();
        deque(int n, const value_type& value);
        deque(long n, const value_type& value);
        deque(size_type n, const value_type& value);
        // 析构函数
        ~deque();
        // 注意这里没有写拷贝构造函数和重载operator= 不要使用这两个函数

        // 功能实现
        void push_back(const value_type& x);
        void push_front(const value_type& x);
        void pop_back();
        void pop_front();
        void clear();
        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);
        iterator insert(iterator position, const value_type& x);

    };

    template<class T, class Alloc, size_t BufSize>
    typename deque<T, Alloc, BufSize>::pointer deque<T, Alloc, BufSize>::allocate_node() {
        // 配置缓冲区空间
        pointer result = data_allocator::allocate(buffer_size());
        return result;
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::deallocate_node(pointer x) {
        // 配置缓冲区空间
        data_allocator::deallocate(x, buffer_size());
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) {
        // 需要的节点数
        // 如果刚好整除, 则多分配一个节点
        size_type num_nodes = num_elements / buffer_size() + 1;

        // 一个 map 要管理几个节点, 最小 8 个, 最多是 num_nodes + 2
        map_size = (num_nodes + 2) < 8 ? 8 : (num_nodes + 2);
        map = map_allocator::allocate(map_size); // 配置 map 空间

        // 以下保证 nstart 和 nfinish 保持在区段的中心位置
        // 使头尾扩充能力一样大
        // 最低也有一个 node
        map_pointer nstart = map + (map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;

        map_pointer cur;
        try {
            // 为 map 内的现用节点配置每个缓冲区
            // 最后一个缓冲区可能会有盈余
            for (cur = nstart; cur <= nfinish; ++cur) {
                *cur = allocate_node();
            }
        }
        catch(...) {
            // 捕获所有错误
            map_pointer cur_cerr;
            // 释放内存
            for (cur_cerr = nstart; cur_cerr < cur; ++cur_cerr) {
                data_allocator::deallocate(*cur_cerr, buffer_size());
            }
            // 释放 map 空间
            map_allocator::deallocate(map, map_size);
            throw;
        }

        start.set_node(nstart);
        finish.set_node(nfinish);
        start.cur = start.first; // 第一个元素
        finish.cur = finish.first + num_elements % buffer_size(); // 最后一个元素的下一位置
        // 前面说刚好整除要多配置一个节点, 此时 cur 刚好指向新缓冲区的 first
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value) {
        create_map_and_nodes(n);
        map_pointer cur;
        try {
            for (cur = start.node; cur < finish.node; ++cur) {
                uninitialized_fill(*cur, *cur + buffer_size(), value);
            }
            // 最后一个节点稍有不同
            uninitialized_fill(finish.first, finish.cur, value);
        }
        catch(...) {
            map_pointer cur_cerr;
            for (cur_cerr = start.node; cur_cerr < cur; ++cur_cerr) {
                // 析构对象
                destroy(*cur_cerr, *cur + buffer_size());
                // 释放空间
                data_allocator::deallocate(*cur_cerr, buffer_size());
            }
            // 释放未构造对象的空间
            for ( ; cur_cerr < finish.node; ++cur_cerr) {
                data_allocator::deallocate(*cur_cerr, buffer_size());
            }
            // 释放 map 空间
            map_allocator::deallocate(map, map_size);
            throw;
        }
    }

    template<class T, class Alloc, size_t BufSize>
    deque<T, Alloc, BufSize>::deque(int n, const value_type& value) : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value);
    }

    template<class T, class Alloc, size_t BufSize>
    deque<T, Alloc, BufSize>::deque(long n, const value_type& value) : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value);
    }

    template<class T, class Alloc, size_t BufSize>
    deque<T, Alloc, BufSize>::deque(size_type n, const value_type& value) : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value);
    }

    template<class T, class Alloc, size_t BufSize>
    deque<T, Alloc, BufSize>::deque() : start(), finish(), map(0), map_size(0) {
        fill_initialize(0, 0);
    }

    template<class T, class Alloc, size_t BufSize>
    deque<T, Alloc, BufSize>::~deque() {
        // 析构每个对象
        destroy(start, finish);
        // 释放每个缓冲区内存 必定存在一个缓冲区
        for (map_pointer cur = start.node; cur <= finish.node; ++cur) {
            deallocate_node(*cur);
        }
        // 释放 map
        map_allocator::deallocate(map, map_size);
    }


    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
        size_type old_num_nodes = finish.node - start.node + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_nstart;
        if (map_size > 2 * new_num_nodes) {
            // 如果 map_size(前端 / 后端有很长的节点还没用) 的空间足够
            // 不用重新配置, 只用重新设定 start 的位置
            new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0); // 如果是前面添加, 那么new_nstart往后再退一格
            if (new_nstart < start.node) {
                copy(start.node, finish.node + 1, new_nstart);
            }
            else {
                copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
            }
        }
        else {
            // 需要重新配置 map , 扩展至两倍以上
            size_type new_map_size = map_size + (nodes_to_add < map_size ? map_size : nodes_to_add) + 2;
            // 配置新的空间
            map_pointer new_map = map_allocator::allocate(new_map_size);
            // 设定新的 start
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            // copy 原来的内容
            copy(start.node, finish.node + 1, new_nstart);
            // 释放原来的 map
            map_allocator::deallocate(map, map_size);
            // 设定新的 map 和 map_size
            map = new_map;
            map_size = new_map_size;
        }
        // 更新 start 和 finish
        start.set_node(new_nstart);
        finish.set_node(new_nstart + old_num_nodes - 1);
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::reserve_map_at_back(size_type nodes_to_add) {
        // 尾端节点不足, 配置一个更大的空间, 拷贝原来的, 释放原来的
        if (nodes_to_add + 1 > map_size - (finish.node - map)) {
            reallocate_map(nodes_to_add, false);
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::reserve_map_at_front(size_type nodes_to_add) {
        // 前端节点不足, 配置一个更大的空间, 拷贝原来的, 释放原来的
        if (nodes_to_add > start.node - map) {
            reallocate_map(nodes_to_add, true);
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_back(const value_type& x) {
        if (finish.cur != finish.last - 1) {
            // 缓冲区还有备用空间, 直接插入
            construct(finish.cur, x);
            ++finish.cur;
        }
        else {
            push_back_aux(x); // 最后缓冲区只剩一个元素备用空间
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_back_aux(const value_type& x) {
        value_type x_copy = x;
        reserve_map_at_back(); // map 中节点空间不足时 重新配置 map 空间
        *(finish.node + 1) = allocate_node(); // 配置一个新的缓冲区
        try {
            construct(finish.cur, x_copy); // 构造对象
            finish.set_node(finish.node + 1); // 切换到下一个缓冲区(因为是最后元素的下一位置)
            finish.cur = finish.first;  // 更新 cur
        }
        catch(...) {
            // 如果不成功 释放新的那一段缓冲区
            deallocate_node(*(finish.node + 1));
            throw;
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_front(const value_type& x) {
        if (start.cur != start.first) {
            // 前端还有备用空间
            construct(start.cur - 1, x);
            --start.cur;
        }
        else {
            push_front_aux(x); // 前端没有备用空间了
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_front_aux(const value_type& x) {
        value_type x_copy = x;
        reserve_map_at_front(); // map 中节点空间不足时, 重新配置 map 空间
        *(start.node - 1) = allocate_node(); // 配置一个新的缓冲区
        try {
            start.set_node(start.node - 1); // 切换到上一个缓冲区
            start.cur = start.last - 1; // 更新cur
            construct(start.cur, x_copy); // 构造对象
        }
        catch(...) {
            // 若非成功, 新增的东西不留下来
            start.set_node(start.node + 1);
            start.cur = start.first;
            deallocate_node(*(start.node - 1));
            throw;
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_back() {
        if (finish.cur != finish.first) {
            --finish.cur;
            destroy(finish.cur); // 析构最后一个元素
        }
        else {
            pop_back_aux(); // 最后缓冲区没有元素
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_back_aux() {
        deallocate_node(finish.first); // 释放最后一个缓冲区
        finish.set_node(finish.node - 1); // 调整 finish 的状态
        finish.cur = finish.last - 1; // 指向最后一个元素
        destroy(finish.cur); // 释放最后一个元素
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_front() {
        if (start.cur != start.last - 1) {
            // 第一缓冲区有两个或更多元素
            destroy(start.cur); // 析构对象
            ++start.cur; // 调整指针
        }
        else {
            pop_front_aux(); // 第一个缓冲区只有一个元素
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_front_aux() {
        destroy(start.cur); // 析构第一个元素
        deallocate_node(start.first); // 释放第一个缓冲区
        start.set_node(start.node + 1); // 设置 start 状态
        start.cur = start.first; // 下一个缓冲区的第一个元素
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::clear() {
        // deque 的最初状态是 最少也有一个缓冲区
        // 以下处理除了头尾的区间
        for (map_pointer node = start.node + 1; node < finish.node; ++node) {
            destroy(*node, *node + buffer_size()); // 析构对象
            deallocate_node(*node); // 释放内存
        }
        if (start.node != finish.node) {
            // 头尾都有一个缓冲区
            destroy(start.cur, start.last); // 析构头缓冲区
            destroy(finish.first, finish.cur); // 析构尾缓冲区
            // 释放尾缓冲区 保留头缓冲区
            deallocate_node(*(finish.node));
        }
        else {
            // 只有一个缓冲区
            destroy (start.cur, finish.cur); // 析构全部元素
            // 不释放内存
        }
        finish = start; // 调整状态
    }

    template<class T, class Alloc, size_t BufSize>
    typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator position) {
        iterator next = position;
        ++next;
        difference_type index = position - start; // 清除点前的元素个数
        if (index < (size() >> 1)) {
            // 如果清除点之前的元素较少
            copy_backward(start, position, next); // 因为迭代器定义了相应的运算符, 所以可以直接用 copy 算法
            pop_front(); // 去掉前面一个
        }
        else {
            // 清除点后的元素较少
            copy(next, finish, position);
            pop_back(); // 去掉最后一个
        }
        return start + index; // 不能返回 next
    }

    template<class T, class Alloc, size_t BufSize>
    typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator first, iterator last) {
        if (first == start && finish == last) {
            clear(); // 是整个区间
            return finish;
        }
        else {
            difference_type n = last - first; // 待删除区间长度
            difference_type elems_before = first - start; // 前端的长短
            if (elems_before < (size() - n) / 2) {
                // 前方元素比较少
                copy_backward(start, first, last); // 向后移动元素
                iterator new_start = start + n; // 标记新起点
                destroy(start, new_start); // 析构前段元素
                // 释放冗余的缓冲区
                for (map_pointer cur = start.node; cur < new_start.node; ++cur) {
                    deallocate_node(*cur);
                }
                start = new_start; // 更新 start
            }
            else {
                // 后方元素较少
                copy(last, finish, first); // 向前移动元素
                iterator new_finish = finish - n; // 标记新尾点
                destroy(new_finish, finish); // 析构后段元素
                // 释放冗余的缓冲区
                for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur) {
                    deallocate_node(*cur);
                }
                finish = new_finish; // 更新 finish
            }
            return start + elems_before;
        }
    }

    template<class T, class Alloc, size_t BufSize>
    typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert(iterator position, const value_type& x) {
        if (position == start) {
            // 插入端是第一个元素
            push_front(x); // 调用 push_front()
            return start;
        }
        else if (position == finish) {
            // 插入位置是最后一个元素
            push_back(x); // 调用 push_back()
            iterator tmp = finish - 1;
            return tmp;
        }
        else {
            return insert_aux(position, x);
        }
    }

    template<class T, class Alloc, size_t BufSize>
    typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert_aux(iterator position, const value_type& x) {
        difference_type index = position - start; // 插入位置前面的元素个数
        value_type x_copy = x;
        if (index < (size() >> 1)) {
            // 前面的元素较少
            push_front(front()); // 在前端插入第一个元素, 可以应对缓冲区是否应该添加的问题
            iterator front1 = start;
            ++front1; 
            iterator front2 = front1;
            ++front2; // 用递增是因为 2 个迭代器都要用
            position = start + index; // 更新插入位置 position
            iterator pos1 = position;
            ++pos1; // pos1 就相当于原来的 position
            copy(front2, pos1, front1); // 元素移动
        }
        else {
            // 后面的元素较少
            push_back(back()); // 在最尾端插入最后一个元素, 可以应对缓冲区是否应该添加的问题
            iterator back1 = finish;
            --back1;
            iterator back2 = back1;
            --back2; // 用递减是因为 2 个迭代器都要用
            position = start + index; // 确保最后的 position 是正确的插入位置
            copy_backward(position, back2, back1); // 元素移动
        }
        *position = x_copy; // 在插入点插入新值
        return position;
    }

}





#endif