#ifndef LI_VECTOR_H_
#define LI_VECTOR_H_

#include "li_alloc.h"
#include "li_uninitialized.h"

namespace LI {
    // vector 容器的实现
    template <class T, class Alloc = alloc>
    class vector {
    public:
        // 嵌套型别定义
        typedef T            value_type;
        typedef value_type*  pointer;
        typedef value_type*  iterator;  // 迭代器就是指针
        typedef value_type&  reference;
        typedef size_t       size_type;
        typedef ptrdiff_t    difference_type;

    protected:
        typedef simple_alloc<T, Alloc> data_allocator; // 默认使用第二级内存配置器
        iterator start;          // 表示目前使用空间的头
        iterator finish;         // 表示目前使用空间的尾
        iterator end_of_storage; // 表示目前可用空间的尾
        // 插入元素的辅助函数 或 没有备用空间时 调用
        void insert_aux(iterator position, const T& x);

        // 负责配置空间并填满内容
        iterator allocate_and_fill(size_type n, const T& x) {
            iterator result = data_allocator::allocate(n);
            uninitialized_fill_n(result, n, x); // 全局函数, 负责在未初始化空间上初始化
            return result;
        }
        // 负责释放内存
        void deallocate() {
            if (start) {
                data_allocator::deallocate(start, end_of_storage - start);
            }
        }
        // 用于构造函数
        void fill_and_initialize(size_type n, const T& value) {
            start = allocate_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }

    public:
        // 构造函数
        vector() : start(0), finish(0), end_of_storage(0) { }
        vector(size_type n, const T& value) { fill_and_initialize(n, value); }
        vector(int n, const T& value) { fill_and_initialize(n, value); }
        vector(long n, const T& value) { fill_and_initialize(n, value); }
        explicit vector(size_type n) { fill_and_initialize(n, T()); }

        // 析构函数
        ~vector() {
            destroy(start, finish); // 析构对象
            deallocate(); // 释放空间
        }
        // 注意这里没有写拷贝构造函数和重载operator= 不要使用这两个函数

        iterator begin() { 
            return start;
        }
        iterator end() {
            return finish;
        }
        size_type size() const {
            return size_type(finish - start);
        }
        size_type capacity() const {
            return size_type(end_of_storage - start);
        }
        bool empty() const {
            return start == finish;
        }

        reference operator[](size_type n) {
            return *(begin() + n);
        }

        reference front() {
            return *begin(); // 第一个元素
        }
        reference back() {
            return *(end() - 1); // 最后一个元素
        }

        // 在源码文件中实现的函数
        void push_back(const T& value);
        void pop_back();
        iterator erase(iterator first, iterator last);
        iterator erase(iterator position);
        void insert(iterator position, size_type n, const T& x);
        void insert(iterator position, const T& x);
        void resize(size_type new_size, const T& x);
        void resize(size_type new_size);
        void clear();
        void reserve(size_type n);


    };

    template <class T, class Alloc>
    void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
        if (finish != end_of_storage) {
            // 还有备用空间
            // 在备用空间起始处构造一个元素, 并以 vector 最后一个元素值为其初值
            construct(finish, *(finish - 1));
            ++finish;
            T x_copy = x;
            copy_backward(position, finish - 2, finish - 1); // 往后copy
            *position = x_copy;
        }
        else {
            // 没有备用空间
            const size_type old_size = size();
            const size_type new_size = old_size != 0 ? 2 * old_size : 1;
            // 如果原大小不为 0 则申请两倍的空间, 为 0 则申请 1 空间
            // 前半段用来放原数据, 后半段放置新数据

            iterator new_start = data_allocator::allocate(new_size);
            iterator new_finish = new_start;
            try {
                new_finish = uninitialized_copy(start, position, new_start);
                // 新元素设定初值
                construct(new_finish, x);
                ++new_finish;
                // 本函数可以被 insert 函数调用, 所以要 copy 后半段
                new_finish = uninitialized_copy(position, finish, new_finish);
            }
            catch(...) { 
                // 捕获所有类型的错误
                destroy(new_start, new_finish); // 析构对象
                data_allocator::deallocate(new_start, new_size); // 释放内存
                throw; // 重新抛出错误
            }

            // 析构并释放原vector
            destroy(begin(), end());
            deallocate(); // 释放内存

            // 调整
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + new_size;
        }
    }


    template <class T, class Alloc>
    void vector<T, Alloc>::push_back(const T& value) {
        if (finish != end_of_storage) {
            // 还有备用空间
            construct(finish, value); // 全局函数
            ++finish;
        }
        else {
            // 没有备用空间
            insert_aux(end(), value);
        }
    }

    template <class T, class Alloc>
    void vector<T, Alloc>::pop_back() {
        --finish;
        destroy(finish);
    }

    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last) {
        iterator i = copy(last, finish, first);
        destroy(i, finish);
        finish = finish - (last - first); // 更新finish
        return first;
    }

    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position) {
        if (position + 1 != end()) {
            copy(position + 1, finish, position);
        }
        --finish;
        destroy(finish);
        return position;
    }

    template <class T, class Alloc>
    void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
        if (n != 0) {
            if (size_type(end_of_storage - finish) >= n) {
                // 有足够的空间
                T x_copy = x;
                // 计算插入点之后的现有元素个数
                const size_type elems_after = finish - position;
                iterator old_finish = finish;
                if (elems_after > n) {
                    // "插入点之后的现有元素个数" 大于 "新增元素个数"
                    // 把现有元素后 n 个元素复制到 未初始化空间
                    uninitialized_copy(finish - n, finish, finish);
                    // 把 elems_after - n 个元素 后移
                    copy_backward(position, old_finish - n, old_finish);
                    // 插入
                    fill_n(position, position + n, x_copy);
                }
                else {
                    // "插入点之后的现有元素个数" 小于等于 "新增元素个数"
                    // 在 finish 开始, 在 未初始化空间 构建 n - elems_after 个新增元素
                    uninitialized_fill_n(finish, n - elems_after, x_copy);
                    finish += n - elems_after;
                    // 把 后 n 个现有元素 复制到 未初始化空间
                    uninitialized_copy(position, old_finish, finish);
                    finish += elems_after;
                    // 填充剩余的新增元素
                    fill(position, old_finish, x_copy);
                }
            }
            else {
                // 空间不足
                // 新空间长度
                const size_type old_size = size();
                const size_type new_size = old_size > n ? 2 * old_size : old_size + n;

                // 配置新的空间
                iterator new_start = data_allocator::allocate(new_size);
                iterator new_finish = new_start;
                try {
                    // 先复制现有元素的前半段
                    new_finish = uninitialized_copy(start, position, new_start);
                    // 填入要插入的元素
                    new_finish = uninitialized_fill_n(new_finish, n, x);
                    // 复制现有元素后半段
                    new_finish = uninitialized_copy(position, finish, new_finish);
                }
                catch (...) {
                    destroy(new_start, new_finish); // 析构对象
                    data_allocator::deallocate(new_start, new_size); // 释放空间
                    throw; // 重新抛出错误
                }
                // 析构并释放旧的vector
                destroy(start, finish);
                deallocate();

                // 调整
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + new_size;
            }
        }
    }

    template <class T, class Alloc> 
    void vector<T, Alloc>::insert(iterator position, const T& x) {
        // 调用 重载版本的 insert
        insert(position, 1, x);
    }

    template <class T, class Alloc> 
    void vector<T, Alloc>::resize(size_type new_size, const T& x) {
        if (new_size < size()) {
            erase(begin() + new_size, end());
        }
        else {
            insert(end(), new_size - size(), x);
        }
    }

    template <class T, class Alloc> 
    void vector<T, Alloc>::resize(size_type new_size) {
        resize(new_size, T());
    }

    template <class T, class Alloc> 
    void vector<T, Alloc>::clear() {
        erase(begin(), end());
    }

    template <class T, class Alloc> 
    void vector<T, Alloc>::reserve(size_type n) {
        if (n > size()) {
            iterator new_start = data_allocator::allocate(n);
            iterator new_finish = new_start;
            if (finish > start) {
                try {
                    // 复制现有元素
                    new_finish = uninitialized_copy(start, finish, new_start);
                }
                catch (...) {
                    destroy(new_start, new_finish); // 析构对象
                    data_allocator::deallocate(new_start, n); // 释放空间
                    throw; // 重新抛出错误
                }
                // 析构原来的vector
                destroy(start, finish);
                deallocate();
                // 调整
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + n;
            }
        }
    }
}

#endif