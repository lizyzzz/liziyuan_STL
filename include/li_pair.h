#ifndef LI_PAIR_H_
#define LI_PAIR_H_

// pair 的实现
namespace LI {

    template <class T1, class T2>
    struct pair {
        typedef T1 first_type;
        typedef T2 second_type;
        
        T1 first;
        T2 second;
        pair(): first(T1()), second(T2()) { }
        pair(const T1& t1, const T2& t2): first(t1), second(t2) { }
        pair(const pair<T1, T2>& p): first(p.first), second(p.second) { }
        pair(const pair<const T1, const T2>& p): first(p.first), second(p.second) { }
        pair(const pair<T1, const T2>& p): first(p.first), second(p.second) { }
        pair(const pair<const T1, T2>& p): first(p.first), second(p.second) { }
        
    };

    template <class T1, class T2>
    struct pair<const T1, T2> {
        typedef T1 first_type;
        typedef T2 second_type;
        
        T1 first;
        T2 second;
        pair(): first(T1()), second(T2()) { }
        pair(const T1& t1, const T2& t2): first(t1), second(t2) { }
        pair(const pair<T1, T2>& p): first(p.first), second(p.second) { }
        pair(const pair<const T1, const T2>& p): first(p.first), second(p.second) { }
        pair(const pair<T1, const T2>& p): first(p.first), second(p.second) { }
        pair(const pair<const T1, T2>& p): first(p.first), second(p.second) { }
        
    };

    template <class T1, class T2>
    struct pair<T1, const T2> {
        typedef T1 first_type;
        typedef T2 second_type;
        
        T1 first;
        T2 second;
        pair(): first(T1()), second(T2()) { }
        pair(const T1& t1, const T2& t2): first(t1), second(t2) { }
        pair(const pair<T1, T2>& p): first(p.first), second(p.second) { }
        pair(const pair<const T1, const T2>& p): first(p.first), second(p.second) { }
        pair(const pair<T1, const T2>& p): first(p.first), second(p.second) { }
        pair(const pair<const T1, T2>& p): first(p.first), second(p.second) { }
        
    };

    template <class T1, class T2>
    struct pair<const T1, const T2> {
        typedef T1 first_type;
        typedef T2 second_type;
        
        T1 first;
        T2 second;
        pair(): first(T1()), second(T2()) { }
        pair(const T1& t1, const T2& t2): first(t1), second(t2) { }
        pair(const pair<T1, T2>& p): first(p.first), second(p.second) { }
        pair(const pair<const T1, const T2>& p): first(p.first), second(p.second) { }
        pair(const pair<T1, const T2>& p): first(p.first), second(p.second) { }
        pair(const pair<const T1, T2>& p): first(p.first), second(p.second) { }
        
    };


}

#endif