#ifndef LI_CONSTRUCT_H_
#define LI_CONSTRUCT_H_

#include <new.h> // 定位 new 表示式
#include "li_type_traits.h"
namespace LI {
    // 负责 构造和析构对象
    
    template <class T1, class T2>
    inline void construct(T1* p, T2& value) {
        new(p) T1(value); // 以 value 为参数在 p 地址上构造 T1对象
    }

    // destroy 的第一版本，接受一个指针
    template <class T>
    inline void destroy(T* pointer) {
        pointer->~T(); // 直接调用析构函数
    }


    // 根据 数据类型 是否有 trivial destructor
    template <class ForwardIterator, class T>
    inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
        typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
        __destroy_aux(first, last, trivial_destructor());
    }

    // 有 non-trivial destructor
    template <class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
        for ( ; first < last; ++first) {
            destroy(&*first); // 解引用迭代器再取地址
        }
    }

    // 有 trivial destructor
    template <class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) { } // 什么都不做
    
    // destroy 的第二版本，接受两个迭代器
    template <class ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last) {
        __destroy(first, last, value_type(first));
    }
    

    // 第二版本针对 char* 和 wchar_t* 的特例化
    inline void destroy(char*, char*){ }
    inline void destroy(wchar_t*, wchar_t*){ }


}





#endif