#ifndef LI_ITERATOR_H_
#define LI_ITERATOR_H_

#include <cstddef>

namespace LI {
    // 五种迭代器类型 空类型 仅做标记类型用
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    // 迭代器基本类型
    template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
    struct iterator {
        typedef Category  iterator_category;
        typedef T         value_type;
        typedef Distance  difference_type;
        typedef Pointer   pointer;
        typedef Reference reference;
    };

    // "榨汁机" traits
    template <class Iterator>
    struct iterator_traits {
        typedef typename Iterator::iterator_category      iterator_category;
        typedef typename Iterator::value_type             value_type;
        typedef typename Iterator::difference_type        difference_type;
        typedef typename Iterator::pointer                pointer;
        typedef typename Iterator::reference              reference;
    };

    // 针对原生指针的 traits 偏特化
    template <class T>
    struct iterator_traits<T*> {
        typedef random_access_iterator_tag      iterator_category;
        typedef T                               value_type;
        typedef ptrdiff_t                       difference_type;
        typedef T*                              pointer;
        typedef T&                              reference;
    };

    // 针对原生之 pointer to const 而设计的偏特化
    template <class T>
    struct iterator_traits<const T*> {
        typedef random_access_iterator_tag      iterator_category;
        typedef T                               value_type;
        typedef ptrdiff_t                       difference_type;
        typedef const T*                        pointer;
        typedef const T&                        reference;
    };

    // 这个函数可以萃取出某个迭代器的类型
    template <class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&) {
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();
    }

    // 这个函数可以萃取出某个迭代器的 distance type (注意返回的是指针类型)
    template <class Iterator>
    inline typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    // 这个函数可以萃取出某个迭代器的 value type (注意返回的是指针类型)
    template <class Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    // 以下是整组 distance 函数
    // input_iterator
    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++first; ++n;
        }
        return n;
    }

    // random_access_iterator
    template<class RandomAccessiterator>
    inline typename iterator_traits<RandomAccessiterator>::difference_type
    __distance(RandomAccessiterator first, RandomAccessiterator last, random_access_iterator_tag) {
        return last - first;
    }

    // 封装
    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
        typedef typename iterator_traits<InputIterator>::iterator_category category;
        return __distance(first, last, category());
    }

    // 以下是整组 advance 函数
    // forward iterator
    template<class InputIterator, class Distance>
    inline void __advance(InputIterator& i, Distance n, input_iterator_tag) {
        while (n--) ++i;
    }
    // bidirectional iterator
    template<class BidirectionalIterator, class Distance>
    inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
        if (n >= 0)
            while (n--) ++i;
        else
            while (n++) --i;
    }
    // random_access iterator
    template<class RandomAccessIterator, class Distance>
    inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
        i += n;
    }

    // 封装
    template<class InputIterator, class Distance>
    inline void advance(InputIterator& i, Distance n) {
        __advance(i, n, iterator_category(i));
    }

}





#endif