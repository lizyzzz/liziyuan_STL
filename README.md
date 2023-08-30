# liziyuan_STL
这是一个包含C++ STL中vector deque map三种容器实现细节以及原理的粗略项目

  实现了STL的六大组件中的部分功能：
### 1. 空间分配器
* (1)内存的分配回收功能：包括内存池的实现(li_alloc.h)
* (2)对象的构造析构功能：(li_construct.h 和 li_uninitialized.h)

### 2. 迭代器
* (1)型别萃取特性：迭代器型别萃取(li_iterator.h)和类型型别萃取(li_type_traits.h)
### 3. 容器
* (1)vector容器(li_vector.hpp)：
&emsp;1.1) 连续内存的分配机制  
&emsp;1.2) 增删导致的内存重分配
* (2)deque容器(li_deque.hpp):  
&emsp;2.1) deque的迭代器(li_deque_iterator.hpp)：自定义操作符以及定义缓冲区跳变的操作(重要), 增加 node 节点维护当前节点所在的缓冲区  
&emsp;2.2) 用中控器实现形式上前后连续的内存空间(li_deque.hpp)  
&emsp;2.3) 内存分配原理：map中控器每个元素指向每个缓冲区的首地址, 每个缓冲区单独申请内存  
* (3)map容器(li_map.hpp)：  
&emsp;3.1) 红黑树具体实现(li_rbtree.hpp)：  
&emsp;&emsp;(a)红黑树的节点两层结构(单独用模板封装数据类型T)；  
&emsp;&emsp;(b)红黑树迭代器(定义operator++和operator--)；  
&emsp;&emsp;(c)红黑树的插入和删除(insert的四种情况，其实用了一个自下而上的程序将情况变为2种；erase的四种情况，本质原来是将双黑节点向上传递到红色节点)。  
&emsp;3.2) 封装红黑树
### 4. 算法
* 实现了 copy 和 copy_backward, fill 和 fill_n (li_algorithm.h)
### 5. 仿函数
* 实现了 less<T>, identity<T> 和 select1st<Pair> (li_functional.h)
### 6. 适配器
* 适配器本质上就是在迭代器，容器，仿函数的基础上进行封装模板化，使每个组件适配。实现略~
       

谢谢~
