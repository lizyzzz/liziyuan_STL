#ifndef LI_UNINITIALIZED_H_
#define LI_UNINITIALIZED_H_

#include "li_type_traits.h"
#include "li_iterator.h"
#include "li_algorithm.h"
#include "li_construct.h"

namespace LI {
    // 内存基本处理工具

    // 把 [first, last) 区间的元素复制到 result 开始的未初始化空间 --------------------------------

    // 有 POD(Plain Old data) : 标量类型 或 C struct 型别 
    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
        return copy(first, last, result); // 调用 copy 算法
    }
    // 有 non-POD
    template<class InputIterator, class ForwardIterator>
    ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
        ForwardIterator cur = result;
        for ( ; first != last; ++first, ++cur) {
            construct(&*cur, *first); // 一个个构造
        }
        return cur;
    }
    template<class InputIterator, class ForwardIterator, class T>
    inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*) {
        typedef typename __type_traits<T>::is_POD_type is_POD;
        return __uninitialized_copy_aux(first, last, result, is_POD());
    }
    
    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
        return __uninitialized_copy(first, last, result, value_type(first));
    }
    // const char* 特化版本
    inline char* uninitialized_copy(const char* first, const char* last, char* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }
    // const wchar_t* 特化版本
    inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }


    // 把未初始化空间 [first, last) 填上 x  ----------------------------------------


    // POD 型别
    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type) {
        fill(first, last, x); // 调用 fill 算法
    }
    // non-POD 型别
    template <class ForwardIterator, class T>
    void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type) {
        ForwardIterator cur = first;
        for ( ; cur != last; ++cur) {
            construct(&*cur, x); // 一个个构造
        }
    }
    // 根据是否是 POD 类型确定使用函数
    template <class ForwardIterator, class T, class T1>
    inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*) {
        typedef typename __type_traits<T1>::is_POD_type is_POD;
        __uninitialized_fill_aux(first, last, x, is_POD());
    }

    template <class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
        __uninitialized_fill(first, last, x, value_type(first));
    }

    

    // 从 first 开始的 n 个位置填上 x ---------------------------------

    // POD 型别
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type) {
        return fill_n(first, n, x); // 调用 fill_n 函数
    }
    // non-POD 型别
    template <class ForwardIterator, class Size, class T>
    ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type) {
        ForwardIterator cur = first;
        for ( ; n > 0; --n, ++cur) {
            construct(&*cur, x);
        }
        return cur;
    }
    // 根据是否是 POD 型别选择函数
    template <class ForwardIterator, class Size, class T, class T1>
    inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*) {
        typedef typename __type_traits<T1>::is_POD_type is_POD;
        return __uninitialized_fill_n_aux(first, n, x, is_POD());
    }

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
        return __uninitialized_fill_n(first, n, x, value_type(first));
    }
}



#endif