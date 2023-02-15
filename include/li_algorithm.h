#ifndef LI_ALGORITHM_H_
#define LI_ALGORITHM_H_

#include "li_iterator.h"
#include "li_type_traits.h"
#include "string.h"
namespace LI {
    // 算法


    // copy 算法-------------------------------------

    // copy 中用到的函数
    template <class RandomAccessIterator, class OutputIterator, class Distance>
    inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*) {
        // 以 n 决定 循环的执行次数, 速度快
        for (Distance n = last - first; n > 0; --n, ++result, ++first) {
            *result = *first;
        }
        return result;
    }
    // InputIterator 版本
    template <class InputIterator, class OutputIterator>
    inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
        // 需要判断迭代器等同与否, 决定循环是否继续, 速度慢
        for ( ; first != last; ++first, ++result) {
            *result = *first;
        }
        return result;
    }
    // RandomAccessIterator 版本
    template <class InputIterator, class OutputIterator>
    inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, random_access_iterator_tag) {
        return __copy_d(first, last, result, distance_type(first));
    }

    template<class InputIterator, class OutputIterator>
    struct __copy_dispatch {
        OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
            return __copy(first, last, result, iterator_category(first));
        }
    };
    

    // 有 trivial assignment operator
    template <class T>
    inline T* __copy_t(const T* first, const T* last, T* result, __true_type) {
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }
    // 有 non-trivial assignment operator
    template <class T>
    inline T* __copy_t(const T* first, const T* last, T* result, __false_type) {
        return __copy_d(first, last, result, (ptrdiff_t*) 0); // 原生指针是一种RandomAccessIterator
    }
    
    // 偏特化版本
    template <class T>
    struct __copy_dispatch<T*, T*> {
        T* operator()(T* first, T* last, T* result) {
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;
            return __copy_t(first, last, result, t());
        }
    };
    template <class T>
    struct __copy_dispatch<const T*, T*> {
        T* operator()(const T* first, const T* last, T* result) {
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;
            return __copy_t(first, last, result, t());
        }
    };
    

    // copy 函数
    template <class InputIterator, class OutputIterator>
    inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
    }
    // 特殊版本
    inline char* copy(const char* first, const char* last, char* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }
    inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }
    

    // copy_backward 中用到的函数 -------------------------------------------
    template<class BidirectionalIterator1, class BidirectionalIterator2>
    struct __copy_backward_dispatch {
        BidirectionalIterator2 operator()(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
            return __copy_backward(first, last, result, iterator_category(first));
        }
    };
    // BidirectionalIterator 版本
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 __copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result, bidirectional_iterator_tag) {
        // 需要判断迭代器等同与否, 决定循环是否继续, 速度慢
        for ( ; last != first; --result) {
            *result = *(--last);
        }
        return result;
    }
    // RandomAccessIterator 版本
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 __copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result, random_access_iterator_tag) {
        return __copy_d_backward(first, last, result, distance_type(first));
    }
    template <class RandomAccessIterator, class OutputIterator, class Distance>
    inline OutputIterator __copy_d_backward(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*) {
        // 以 n 决定 循环的执行次数, 速度快
        for (Distance n = last - first; n > 0; --n, --result) {
            *result = *(--last);
        }
        return result;
    }

    // 偏特化版本
    template <class T>
    struct __copy_backward_dispatch<T*, T*> {
        T* operator()(T* first, T* last, T* result) {
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;
            return __copy_t_backward(first, last, result, t());
        }
    };
    template <class T>
    struct __copy_backward_dispatch<const T*, T*> {
        T* operator()(const T* first, const T* last, T* result) {
            typedef typename __type_traits<T>::has_trivial_assignment_operator t;
            return __copy_t_backward(first, last, result, t());
        }
    };
    // 有 trivial assignment operator
    template <class T>
    inline T* __copy_t_backward(const T* first, const T* last, T* result, __true_type) {
        memmove(result - (last - first), first, sizeof(T) * (last - first));
        return result - (last - first);
    }
    // 有 non-trivial assignment operator
    template <class T>
    inline T* __copy_t_backward(const T* first, const T* last, T* result, __false_type) {
        return __copy_d_backward(first, last, result, (ptrdiff_t*) 0); // 原生指针是一种RandomAccessIterator
    }
    // copy_backward 函数
    template<class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
        return __copy_backward_dispatch<BidirectionalIterator1, BidirectionalIterator2>()(first, last, result);
    }
    // 特殊版本
    inline char* copy_backward(const char* first, const char* last, char* result) {
        memmove(result - (last - first), first, last - first);
        return result - (last - first);
    }
    inline wchar_t* copy_backward(const wchar_t* first, const wchar_t* last, wchar_t* result) {
        memmove(result - (last - first), first, sizeof(wchar_t) * (last - first));
        return result - (last - first);
    }

    // fill 算法 -----------------------------------------------
    template <class ForwardIterator, class T>
    void fill (ForwardIterator first, ForwardIterator last, const T& value) {
        for ( ; first != last; ++first) {
            *first = value;
        }
    }

    // fill_n 算法 ----------------
    template <class ForwardIterator, class Size, class T>
    ForwardIterator fill_n (ForwardIterator first, Size n, const T& value) {
        for ( ; n > 0; --n, ++first) {
            *first = value;
        }
        return first;
    }




}


#endif