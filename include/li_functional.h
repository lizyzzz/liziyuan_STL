#ifndef LI_FUNCTIONAL_H_
#define LI_FUNCTIONAL_H_

namespace LI {
    // 用来呈现 一元函数的参数型别和返回值型别
    template <class Arg, class Result>
    struct unary_function {
        typedef Arg argument_type;
        typedef Result result_type;
    };

    // 用来呈现 二元函数的参数型别和返回值型别
    template <class Arg1, class Arg2, class Result>
    struct binary_function {
        typedef Arg1 first_argument_type;
        typedef Arg2 second_argument_type;
        typedef Result result_type;
    };

    template <class T>
    struct less : public binary_function<T, T, bool> {
        bool operator()(const T& x, const T& y) const { return x < y; }
    };
    
    template <class T>
    struct identity : public unary_function<T, T> {
        const T& operator()(const T& x) const { return x; }
    };

    template <class Pair>
    struct select1st : public unary_function<Pair, typename Pair::first_type> {
        const typename Pair::first_type& operator()(const Pair& x) const { return x.first; }
    };







}




#endif