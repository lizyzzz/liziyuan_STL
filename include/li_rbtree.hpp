#ifndef LI_RBTREE_H_
#define LI_RBTREE_H_

#include "li_iterator.h"
#include "li_alloc.h"
#include "li_construct.h"
#include "li_pair.h"

// 红黑树的设计与实现
// 红黑树是一个二叉搜索树, 且满足以下规则:
// 1. 每个节点不是红色就是黑色
// 2. 根节点是黑色
// 3. 如果节点为红, 其子节点必须为黑
// 4. 任一节点至NULL(树尾端)的任何路径, 所含之黑节点数必须相同
//   根据规则 4, 新增节点必须为红; 根据规则 3, 红节点的父节点和子节点必须为黑.
// 

namespace LI {

    // RB-tree 的节点设计
    typedef bool __rb_tree_color_type;
    const __rb_tree_color_type __rb_tree_red = false; // 红色 为 false
    const __rb_tree_color_type __rb_tree_black = true; // 黑色 为 true

    // 节点两层设计
    // 第一层 无值的类型
    struct __rb_tree_node_base {
        typedef __rb_tree_color_type color_type;
        typedef __rb_tree_node_base* base_ptr;

        color_type color; // 节点颜色, 非黑即红
        base_ptr parent; // 父节点
        base_ptr left; // 左孩子
        base_ptr right; // 右孩子

        static base_ptr minimum (base_ptr x) {
            while (x->left != nullptr) x = x->left; // 一直往左走
            return x;
        }

        static base_ptr maximum (base_ptr x) {
            while (x->right != nullptr) x = x->right; // 一直往右走
            return x;
        }

    };
    // 第二层 封装 值的类型
    template <class Value>  // 这个 Value 可以是 pair 用来定义 key value
    struct __rb_tree_node : public __rb_tree_node_base {
        typedef __rb_tree_node<Value>* link_type;
        Value value_field; // 节点值
    };

    // RB-tree 的迭代器
    // 基层迭代器
    struct __rb_tree_base_iterator {
        typedef __rb_tree_node_base::base_ptr base_ptr;
        typedef bidirectional_iterator_tag iterator_category; // 迭代器类型为双向迭代器
        typedef ptrdiff_t difference_type; // 自定义的迭代器要自定义的类型

        base_ptr node; // 节点的指针, 用来与容器之间产生一个连结关系

        void increment(); // operator++的实现

        void decrement(); // operator--的实现

    };

    // 红黑树的一个特殊设计: 增加一个 end 红节点 与根节点互为父节点
    // end 红节点的左孩子指向最小值, 右孩子指向最大值
    // 用于判断状态 
    // 二叉搜索树的中序遍历就是递增的次序, 因此 ++ 按照中序遍历找下一个节点
    void __rb_tree_base_iterator::increment() {
        if (node->right != nullptr) { // 如果有右节点
            node = node->right;       // 往右节点走
            while (node->left != nullptr) {
                node = node->left;    // 找到右子树的最小值
            }
        }
        else {
            base_ptr y = node->parent; // 找到父节点
            while (node == y->right) { // 继续往上直至是左孩子
                node = y;
                y = y->parent;
            }
            // 以下判断排除一种情况
            // 如果能找到左孩子 那么 y 是所求; 如果到达了终点, y 也是所求
            // 但是当根节点是最大值时, 若 当前 node 为根节点, 那么 node 为所求, 不需要再执行 node = y;
            if (node->right != y) {
                node = y;
            }
        }
    }

    // 按中序遍历的次序, -- 是找上一个节点
    void __rb_tree_base_iterator::decrement() {
        if (node->color == __rb_tree_red && node->parent->parent == node) {  // 这个节点是 end 红节点
            node = node->right;
        }
        else if (node->left != nullptr) {  // 存在左子树
            node = node->left;
            // 找到左子树的最大值
            while (node->right != nullptr) {
                node = node->right;
            }
        }
        else {
            base_ptr y = node->parent; // 找到父节点
            while (node == y->left) { // 继续往上 直至是右孩子
                node = y;
                y = y->parent;
            }
            node = y; // 此为所求
            // 即使是特殊情况: node 根节点且是最小值, 即begin()
            // 此种情况下 begin()-- == begin();
        }
    }


    // 正规的迭代器
    template <class Value, class Ref, class Ptr>
    struct __rb_tree_iterator : public __rb_tree_base_iterator {
        typedef Value value_type;  // 迭代器必须定义的 5 种类型
        typedef Ref   reference; // 迭代器必须定义的 5 种类型
        typedef Ptr   pointer; //迭代器必须定义的 5 种类型

        typedef __rb_tree_iterator<Value, Value&, Value*> iterator;
        typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
        typedef __rb_tree_iterator<Value, Ref, Ptr> self;
        typedef __rb_tree_node<Value>* link_type; // 节点的指针

        // 构造函数
        __rb_tree_iterator() {}
        __rb_tree_iterator(link_type x) { node = x; } // 基层类指针指向派生类对象
        __rb_tree_iterator(const iterator& it) { node = it.node; }

        // 运算符重载
        reference operator*() const {  // 因为 key 不可变, 所以是常函数
            return link_type(node)->value_field;
        }
        pointer operator->() const {
            return &(operator*());
        }

        self& operator++() {
            increment();
            return *this;
        }
        self operator++(int) {
            self tmp = *this;
            increment();
            return tmp;
        }

        self& operator--() {
            decrement();
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            decrement();
            return tmp;
        }
        // 无序容器, 没有 < 
        bool operator==(const self& x) const {
            return node == x.node;
        }
        bool operator!=(const self& x) const {
            return !(*this == x);
        }

    };

    // 全局函数
    // 新节点必为红节点. 如果插入处之父节点亦为红节点, 就违反红黑树规则, 此时可能需做树形旋转(及颜色改变, 在程序他处)
    inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
        // x 为旋转点
        __rb_tree_node_base* y = x->right; // 令 y 为旋转点的右子节点
        x->right = y->left;
        if (y->left != nullptr) {
            y->left->parent = x; // 记得设置父节点
        }
        y->parent = x->parent;

        if (x == root) {
            // 如果 x == root, 让 y 完全顶替 x
            root = y;
        }
        else if (x == x->parent->left) { // 如果 x 是左节点
            x->parent->left = y;
        }
        else { // x 是右节点
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    // 全局函数
    // 新节点必为红节点. 如果插入处之父节点亦为红节点, 就违反红黑树规则, 此时可能需做树形旋转(及颜色改变, 在程序他处)
    inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
        // x 为旋转点
        __rb_tree_node_base* y = x->left; // 令 y 为旋转点的左节点
        x->left = y->right;
        if (y->right != nullptr) {
            y->right->parent = x; // 记得设置父节点
        }
        y->parent = x->parent;

        if (x == root) { // x 为根节点
            root = y; // 令 y 代替 x
        }
        else if (x == x->parent->right) {  // x 是右节点
            x->parent->right = y; 
        }
        else { // x 是左节点
            x->parent->left = y; 
        }

        y->right = x;
        x->parent = y;
    }

    // 全局函数
    // 调整树形和颜色, 参数 1 是新增节点, 参数 2 是 root
    // root 的值会被改变
    // 这是一个自下而上的程序, 沿着路径, 如果遇到 x 的两个子节点都是红色, 就把 x 改为红色, 两子节点改为黑色
    // 可以避免 复杂的情况(父节点和祖父节点都是红色)
    inline void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
        x->color = __rb_tree_red; // 新节点必须为红
        // 父节点为红
        while (x != root && x->parent->color == __rb_tree_red) {
            if (x->parent == x->parent->parent->left) { // 父节点是祖父节点的左节点
                __rb_tree_node_base* y = x->parent->parent->right; // 令 y 是伯父节点
                if (y && y->color == __rb_tree_red) { // 伯父节点存在且为红色节点
                    // 发现两个子节点都是红色
                    x->parent->color = __rb_tree_black; // 更改父节点为黑色
                    y->color = __rb_tree_black; // 更改伯父节点为黑色
                    x->parent->parent->color = __rb_tree_red; // 更改祖父节点为红色
                    x = x->parent->parent; // 继续往上检查
                }
                else { // 无伯父节点, 或伯父节点为黑色
                    if (x == x->parent->right) { // 如果新节点为父节点之右子节点
                        x = x->parent;
                        // 如果是右子节点就先左旋
                        __rb_tree_rotate_left(x, root); // 第一参数为左旋点
                    }
                    x->parent->color = __rb_tree_black; // 改变颜色
                    x->parent->parent->color = __rb_tree_red;
                    // 右旋
                    __rb_tree_rotate_right(x->parent->parent, root); // 第一参数为右旋点
                }
            }
            else { // 父节点是祖父节点的右节点
                __rb_tree_node_base* y = x->parent->parent->left; // 令 y 是伯父节点
                if (y && y->color == __rb_tree_red) { // 伯父节点存在且为红色
                    // 发现两个字节点都是红色
                    x->parent->color = __rb_tree_black; // 更改父节点颜色
                    y->color = __rb_tree_black; // 更改伯父节点颜色
                    x->parent->parent->color = __rb_tree_red; // 更改祖父节点的颜色
                    x = x->parent->parent; // 继续往上检查
                }
                else { // 伯父节点不存在 或 伯父节点为黑色
                    if (x == x->parent->left) { // 如果新节点为父节点的左节点
                        x = x->parent;
                        // 如果是左节点就先右旋
                        __rb_tree_rotate_right(x, root);
                    }
                    x->parent->color = __rb_tree_black; // 改变颜色
                    x->parent->parent->color = __rb_tree_red;
                    // 左旋
                    __rb_tree_rotate_left(x->parent->parent, root); // 第一参数是左旋点
                }
            }
        }
        root->color = __rb_tree_black; // 根节点永远为黑
    }

    inline __rb_tree_node_base* __rb_tree_rebalance_for_erase(__rb_tree_node_base* z, 
                                                            __rb_tree_node_base*& root,
                                                            __rb_tree_node_base*& leftmost,
                                                            __rb_tree_node_base*& rightmost) {
        // z 是 删除节点
        __rb_tree_node_base* y = z; // y 是删除节点
        __rb_tree_node_base* x = nullptr; // x 是代替 y 的节点, 可能为 null
        __rb_tree_node_base* x_parent = nullptr; // x_parent 是 x 的父节点 
        // 正常二叉树的删除
        if (y->left == nullptr) { // z 至多只有一个孩子
            x = y->right;  // x 可能是 null
        }
        else { // z 的左孩子不为null
            if (y->right == nullptr) { // z 只有一个左孩子
                x = y->left; // x 非 null
            }
            else { // z 有两个孩子
                y = y->right; // 右子树
                while (y->left != nullptr) { // 左子树的最小值
                    y = y->left;
                }
                x = y->right; // x 可能为 null 也可能不为 null
            }
        }

        if (y != z) { // z 有两个孩子, 用 y 代替 z
            z->left->parent = y; // y 先连接 左子树
            y->left = z->left;
            if (y != z->right) {  // y 不是 z 的右孩子
                x_parent = y->parent;
                if (x) { // 如果 x 不为 null, 先用 x 代替 y 的位置
                    x->parent = y->parent;
                }
                y->parent->left = x; // y 必定是左孩子

                y->right = z->right; // y 连接右子树
                z->right->parent = y;
            }
            else { // y 刚好是 z 的右孩子
                x_parent = y;
            }

            if (root == z) { // z 是根节点
                root = y; // 直接用 y 代替 root
            }
            else if(z->parent->left == z) { // z 是左节点
                z->parent->left = y;
            }
            else { // z 是右节点
                z->parent->right = y;
            }
            y->parent = z->parent; // 更新父节点

            // 交换颜色
            __rb_tree_color_type tmp_color = z->color;
            z->color = y->color;
            y->color = tmp_color;

            y = z; // 做好 y 代替 z 的连接操作后, y 重新指向要删除的节点
        }
        else { // z == y, 即 z 至多一个孩子, x 指向 y 有可能存在的孩子
            // 用 x 代替 y(z) 的位置
            x_parent = y->parent;
            if (x) {
                x->parent = y->parent; 
            }

            if (root == z) { // z == y == root
                root = x; // 直接用 x 代替 root
            }
            else {
                if (z->parent->left == z) {
                    z->parent->left = x;
                }
                else {
                    z->parent->right = x;
                }
            }

            if (leftmost == z) { // 维护最小值
                if (z->right == nullptr) { // 此时 z 的左节点为 null
                    leftmost = z->parent;
                }
                else {
                    leftmost = __rb_tree_node_base::minimum(x); // 此时 x 必为 z 的右节点
                }
            }
            if (rightmost == z) { // 维护最大值
                if (z->left == nullptr) { // 此时 z 的右节点为 null
                    rightmost = z->parent;
                }
                else {
                    rightmost = __rb_tree_node_base::maximum(x); // 此时 x 必为 z 的左节点
                }
            }
        }

        // y 是要删除的节点, y 的颜色是黑色, 需要调整树形
        if (y->color != __rb_tree_red) {
            while (x != root && (x == nullptr || x->color == __rb_tree_black)) { // 双黑
                if (x == x_parent->left) {
                    __rb_tree_node_base* w = x_parent->right; // 兄弟节点, 必定存在, 如果不存在删除前不符合黑高的规则
                    if (w->color == __rb_tree_red) { // 兄弟节点是红色, 其子节点必然是黑色, x_parent 必然是黑色
                        w->color = __rb_tree_black; // 更改颜色
                        x_parent->color = __rb_tree_red;
                        __rb_tree_rotate_left(x_parent, root); // 左旋转
                        w = x_parent->right; // 待删除节点的兄弟节点(转化为兄弟节点为黑色的情况)
                    }
                    // 以下是兄弟节点为黑色的情况
                    if ((w->left == nullptr || w->left->color == __rb_tree_black) && 
                        (w->right == nullptr || w->right->color == __rb_tree_black)) { // 兄弟节点的孩子节点都为黑色
                        w->color = __rb_tree_red; // 更改兄弟的颜色为红色(这样存在风险, 如果x_parent为红色会双红)
                        x = x_parent; // 所以继续往上检查
                        x_parent = x_parent->parent;
                    }
                    else { // 兄弟节点的孩子节点存在红色
                        if (w->right == nullptr || w->right->color == __rb_tree_black) { // 兄弟节点的右节点是黑色
                            if (w->left) {
                                w->left->color = __rb_tree_black; // 兄弟节点的左孩子节点 改为黑色
                            }
                            w->color = __rb_tree_red; // 兄弟节点改为红色
                            __rb_tree_rotate_right(w, root); // 右旋
                            w = x_parent->right; // 变成兄弟节点的右节点是红色的情况
                        }
                        // 兄弟节点的右节点是红色的情况
                        // 交换兄弟节点和父节点的颜色
                        w->color = x_parent->color;
                        x_parent->color = __rb_tree_black; // 兄弟节点必是黑色的
                        if (w->right) {
                            w->right->color = __rb_tree_black; // 右孩子从红色变成黑色的
                        }
                        __rb_tree_rotate_left(x_parent, root); // 左旋
                        break; // 完成操作不必再调整了
                    }
                }
                else { // 与上面的类似, 但 x 是右节点
                    __rb_tree_node_base* w = x_parent->left;
                    if (w->color == __rb_tree_red) {
                        w->color = __rb_tree_black;
                        x_parent->color = __rb_tree_red;
                        __rb_tree_rotate_right(x_parent, root);
                        w = x_parent->left;
                    }

                    if ((w->right == nullptr || w->right->color == __rb_tree_black) &&
                        (w->left == nullptr || w->left->color == __rb_tree_black)) {
                        w->color = __rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    }
                    else {
                        if (w->left == nullptr || w->left->color == __rb_tree_black) {
                            if (w->right) {
                                w->right->color = __rb_tree_black;
                            }
                            w->color = __rb_tree_red;
                            __rb_tree_rotate_left(w, root);
                            w = x_parent->left;
                        }
                        w->color = x_parent->color;
                        x_parent->color = __rb_tree_black;
                        if (w->left) {
                            w->left->color = __rb_tree_black;
                        }
                        __rb_tree_rotate_right(x_parent, root);
                        break;
                    }
                }
            }
            if (x) {
                x->color = __rb_tree_black; // 只有不断往上检查那一步跳出循环时遇到红色, 所以要改成黑色
            }
        }
        return y;

    }

    // 红黑树的实现
    // Value 通常是一个 pair
    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
    class rb_tree {
    protected:
        typedef void* void_pointer;
        typedef __rb_tree_node_base* base_ptr;
        typedef __rb_tree_node<Value> rb_tree_node;
        typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator; // 空间配置器
        typedef __rb_tree_color_type color_type;
    
    public:
        // 迭代器定义在后面
        typedef Key key_type;
        typedef Value value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef rb_tree_node* link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    protected:
        link_type get_node() {
            return rb_tree_node_allocator::allocate(); // 申请一个节点空间
        }
        void put_node(link_type p) {
            rb_tree_node_allocator::deallocate(p); // 释放一个节点空间
        }
        // 构造一个节点
        link_type creat_node (const value_type& x) {
            link_type tmp = get_node();
            try {
                construct(&(tmp->value_field), x);
            }
            catch(...) {
                put_node(tmp);
                throw;
            }
            return tmp;
        }
        // 复制一个节点 包括颜色
        link_type clone_node (link_type x) {
            link_type tmp = creat_node(x->value_field);
            tmp->color = x->color;
            tmp->left = nullptr;
            tmp->right = nullptr;
            return tmp;
        }

        // 析构并释放一个节点
        void destroy_node (link_type p) {
            destroy(&p->value_field); // 析构内容
            put_node(p); // 释放内存
        }

    protected:
        // RB_tree 只以 三笔数据表现
        size_type node_count; // 节点数量
        link_type header; // 实现上的技巧, 指向 end 节点
        Compare key_compare; // 节点间键值的比较准则, 是一个可调用对象

        // 以下三个成员方便取得节点 header 的成员
        link_type& root() const {
            return (link_type&) header->parent;
        }
        link_type& leftmost() const {
            return (link_type&) header->left; // 最小值
        }
        link_type& rightmost() const {
            return (link_type&) header->right; // 最大值
        }

        // 以下六个函数用来方便取得节点 x 的成员
        static link_type& left(link_type x) {
            return (link_type&) (x->left);
        }
        static link_type& right(link_type x) {
            return (link_type&) (x->right);
        }
        static link_type& parent(link_type x) {
            return (link_type&) (x->parent);
        }
        static reference value(link_type x) {
            return x->value_field;
        }
        static const Key& key(link_type x) {
            return KeyOfValue()(value(x)); // 如果 value(x) 是 pair, 则返回第一个值
        }
        static color_type& color(link_type x) {
            return (color_type&) (x->color);
        }

        // 以下六个函数用来方便取得节点 x 的成员
        static link_type& left(base_ptr x) {
            return (link_type&) (x->left);
        }
        static link_type& right(base_ptr x) {
            return (link_type&) (x->right);
        }
        static link_type& parent(base_ptr x) {
            return (link_type&) (x->parent);
        }
        static reference value(base_ptr x) {
            return ((link_type)x)->value_field;
        }
        static const Key& key(base_ptr x) {
            return KeyOfValue()(value(link_type(x))); // 如果 value(x) 是 pair, 则返回第一个值
        }
        static color_type& color(base_ptr x) {
            return (color_type&) (link_type(x)->color);
        }

        // 求取极大值和极小值
        static link_type minimum(link_type x) {
            return (link_type)(__rb_tree_node_base::minimum(x));
        }
        static link_type maximum(link_type x) {
            return (link_type)(__rb_tree_node_base::maximum(x));
        }

    public:
        typedef __rb_tree_iterator<value_type, reference, pointer> iterator; // 迭代器
        typedef __rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator; // 迭代器
    private:
        iterator __insert(base_ptr x_, base_ptr y_, const value_type& v);
        // link_type __copy(link_type x, link_type p);
        // void __erase(link_type x);
        // 初始化的函数
        void init() {
            header = get_node(); // 分配空间
            color(header) = __rb_tree_red; // end 为红色
            root() = nullptr;
            leftmost() = header;  // header 的左节点初始化为自己
            rightmost() = header;  // header 的右节点初始化为自己
        }
        void PostOrder(link_type x);
    public:
        // 默认构造函数
        rb_tree(const Compare& comp = Compare()) : node_count(0), key_compare(comp) {
            init();
        }

        ~rb_tree() {
            clear();
            put_node(header);
        }

        // rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

    public:
        Compare key_comp() const {
            return key_compare;
        }
        iterator begin() {
            return leftmost(); // 会以 leftmost() 返回的 link_type 为参数构造迭代器
        }
        iterator end() {
            return header; // 会以 header 为参数 构造迭代器
        }
        bool empty() const {
            return node_count == 0;
        }
        size_type size() const {
            return node_count;
        }
        size_type max_size() const {
            return size_type(-1); // size_type 是  unsigned 型的, -1 是其正最大值
        }
        void clear() {
            PostOrder(link_type(header->parent));
            leftmost() = header;
            rightmost() = header;
            root() = nullptr;
            node_count = 0;
        }

    public:
        // 将 x 插入 RB-tree (允许重复)
        iterator insert_equal(const value_type& v);
        // 将 x 插入 RB-tree (不允许重复)
        pair<iterator, bool> insert_unique(const value_type& v);

        // 根据键值查找节点
        iterator find(const key_type& k);
        // 根据键值删除节点
        void erase(const key_type& k);
        // 根据迭代器删除节点
        void erase(iterator position);
    };

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::PostOrder(link_type x) {
        if (x) {
            PostOrder((link_type)x->left);
            PostOrder((link_type)x->right);
            destroy_node(x);
        }
    }

    // 返回值是一个迭代器, 指向新增节点
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const value_type& v) {
        link_type y = header;
        link_type x = root();
        while (x != nullptr) {
            y = x;
            x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
            // 找到合适的插入位置
        }

        return __insert(x, y, v);
    }

    // 返回值是一个迭代器 和 一个成功与否的标志
    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool> 
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type& v) {
        link_type y = header;
        link_type x = root();

        bool comp = true;
        while (x != nullptr) {
            y = x;
            comp = key_compare(KeyOfValue()(v), key(x)); // v 的键值小于目前节点的键值?
            x = comp ? left(x) : right(x); // x > v 当前节点往左; x <= v 当前节点往右
        }
        // 离开循环后, y 是插入点的父节点

        iterator j = iterator(y); // 令迭代器 j 指向插入点之父节点 y
        if (comp) { // 如果离开 while 的时候 comp 为真, 则 v 应该插入在左侧(v < j)
            if (j == begin()) { // 如果父节点是最左节点
                return pair<iterator, bool>(__insert(x, y, v), true);
            }
            else {
                --j; // 父节点不是最左节点, 调整 j, 准备回头测试... (--j) < j
            }
        }
        if (key_compare(key(j.node), KeyOfValue()(v))) { // (--j) < v < j
            // 新键值不与既有节点之键值重复, 于是执行以下操作
            return pair<iterator, bool>(__insert(x, y, v), true);
        }

        // 否则 肯定有重复的键值
        // (--j) >= v 且 v < j  ==> 插入位置矛盾  ==> v == (--j)
        // v >= j 且 (--j) >= v  ==> v == j
        return pair<iterator, bool>(j, false);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(base_ptr x_, base_ptr y_, const value_type& v) {
        // 参数 x_ 为插入点, y_ 是插入点的父节点, v 是插入值
        link_type x = (link_type)x_;
        link_type y = (link_type)y_;

        link_type z;

        // y 为 header 或 v < y 或 x != 0(这种情况好像不会出现?) 
        if (y == header || x != nullptr || key_compare(KeyOfValue()(v), key(y))) {
            z = creat_node(v); // 产生一个新节点
            left(y) = z;  // 这里如果 y == header, 也即 leftmost() = z 
            if (y == header) {
                root() = z;
                rightmost() = z;
            }
            else if (y == leftmost()) {
                leftmost() = z; // 维护最小值
            }
        }
        else {
            z = creat_node(v); // 产生一个新节点
            right(y) = z;
            if (y == rightmost()) {
                rightmost() = z; // 维护最大值
            }
        }
        parent(z) = y; // 设定父节点
        left(z) = nullptr;
        right(z) = nullptr;

        // 新节点的颜色在 __rb_tree_rebalance() 设定
        __rb_tree_rebalance(z, header->parent); // 参数 1 是新增节点, 参数 2 是 root
        ++node_count;
        return iterator(z);
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const key_type& k) {
        link_type y = header;
        link_type x = root();
        
        while (x != nullptr) {
            if (!key_compare(key(x), k)) { // x >= k , 往左走
                y = x;
                x = left(x);
            }
            else { // x < k, 往右走
                x = right(x);
            }
        }

        iterator j = iterator(y);
        return (j == end() || key_compare(k, key(j.node))) ? end() : j;
        // j == end() 即 k 是最大的, 不可能相等
        // k < key(j.node) 即不存在相等的, 如果相等, 有 k >= key(j.node)
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position) {
        link_type y = (link_type) __rb_tree_rebalance_for_erase(position.node, header->parent, header->left, header->right);
        destroy_node(y); // 删除节点
        --node_count;
    }

    template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const key_type& k) {
        iterator position = find(k);
        if (position == end()) {
            return;
        }
        erase(position); // 调用另一个版本 erase
    }

}






#endif