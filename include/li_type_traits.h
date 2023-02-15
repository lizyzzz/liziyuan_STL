#ifndef LI_TYPE_TRAITS_H_
#define LI_TYPE_TRAITS_H_

namespace LI {
    // 空类型 (仅做标记用)
    struct __true_type {};
    struct __false_type {};


    template <class type>
    struct __type_traits {
        typedef __true_type this_dummy_member_must_be_first;

        // 五种类型
        typedef __false_type   has_trivial_default_constructor;
        typedef __false_type   has_trivial_copy_constructor;
        typedef __false_type   has_trivial_assignment_operator;
        typedef __false_type   has_trivial_destructor;
        typedef __false_type   is_POD_type;
    };

    // 一些特化版本
    template<>
    struct __type_traits<char> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };
    template<>
    struct __type_traits<signed char> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };
    template<>
    struct __type_traits<unsigned char> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };

    template<>
    struct __type_traits<short> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };
    template<>
    struct __type_traits<unsigned short> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };

    template<>
    struct __type_traits<int> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };
    template<>
    struct __type_traits<unsigned int> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };
    template<>
    struct __type_traits<long> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };
    template<>
    struct __type_traits<unsigned long> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };
    template<>
    struct __type_traits<float> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };
    template<>
    struct __type_traits<double> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };
    template<>
    struct __type_traits<long double> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };
    // 原生指针的偏特化版本
    template<class T>
    struct __type_traits<T*> {
        // 五种类型
        typedef __true_type   has_trivial_default_constructor;
        typedef __true_type   has_trivial_copy_constructor;
        typedef __true_type   has_trivial_assignment_operator;
        typedef __true_type   has_trivial_destructor;
        typedef __true_type   is_POD_type;
    };





}


#endif