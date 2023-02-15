#ifndef LI_MAP_H_
#define LI_MAP_H_

#include "li_rbtree.hpp"
#include "li_pair.h"
#include "li_functional.h"
#include "li_alloc.h"


// map 的实现
namespace LI {
    template <class Key, class T, class Compare = less<Key>, class Alloc = alloc>
    class map {
    public:
        typedef Key key_type; // 键值型别
        typedef T data_type; // 数据型别
        typedef T mapped_type;
        typedef pair<const Key, T> value_type; // 元素型别(键值/实值)
        typedef Compare key_compare; // 键值比较元素

        // 定义一个仿函数, 其作用就是调用 "元素比较函数"
        class value_compare : public binary_function<value_type, value_type, bool> {
        public:
            friend class map<Key, T, Compare, Alloc>; // 友元类
            bool operator()(const value_type& x, const value_type& y) const {
                return comp(x.first, y.first);
            }
        protected:
            Compare comp;
            value_compare(Compare c) : comp(c) { } // protected 属性
        };

    private:
        // 底层以红黑树封装
        typedef rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc> rep_type;
        rep_type t; // 红黑树对象

    public:
        typedef typename rep_type::pointer pointer;
        typedef typename rep_type::const_pointer const_pointer;
        typedef typename rep_type::reference reference;
        typedef typename rep_type::const_reference const_reference;
        typedef typename rep_type::iterator iterator; // 允许修改实值, 不能定义为const_iterator
        typedef typename rep_type::const_iterator const_iterator; 
        typedef typename rep_type::size_type size_type;
        typedef typename rep_type::difference_type difference_type;

        map() : t(Compare()) { } // 只定义默认构造函数, 其他就不定义了

        ~map() { }

        key_compare key_comp() const { return t.key_comp(); }
        value_compare value_comp() const { return value_compare(t.key_comp()); } // 也是以 key_comp 返回
        iterator begin() { return t.begin(); }
        iterator end() { return t.end(); }
        bool empty() const { return t.empty(); }
        size_type size() const { return t.size(); }
        size_type max_size() const { return t.max_size(); }
        T& operator[] (const key_type& k) {
            return (*((insert(value_type(k, T()))).first)).second;
        }

        // 注意 insert 的返回类型
        pair<iterator, bool> insert(const value_type& x) {
            return t.insert_unique(x);
        }
        void erase(iterator position) { t.erase(position); }
        void erase(const key_type& k) { t.erase(k); }
        void clear() { t.clear(); }
        iterator find(const key_type& x) { return t.find(x); }
    };





}


#endif 