#ifndef LI_ALLOC_H_
#define LI_ALLOC_H_

#include <new>
#include <malloc.h>

namespace LI {
    // 负责内存的 配置和释放

    // 第一级配置器
    // 无 template 参数 inst 没用上
    template <int inst>
    class __malloc_alloc_template {
    private:
        // 以下函数用来处理 内存不足 的情况
        // oom : out of memory
        static void *oom_malloc(size_t); // 返回 void*
        static void *oom_realloc(void *, size_t);
        static void (* __malloc_alloc_oom_handler) (); // 函数指针

    public:
        static void* allocate(size_t n) {
            void* result = malloc(n); // 第一级配置器直接用malloc
            if (0 == result) {
                result = oom_malloc(n);
            }
            return result;
        }
        static void deallocate(void* p, size_t) {
            free(p); // 第一级配置器直接用free
        }

        static void* reallocate(void* p, size_t /*old_sze*/, size_t new_sz) {
            void* result = realloc(p, new_sz); // 第一级配置器直接用 realloc()
            // 无法满足要求时用oom_realloc()
            if (0 == result) {
                result = oom_realloc(p, new_sz);
            }
            return result;
        }

        // 开放接口
        // 可以指定自己的 out of memory handler
        static void (* setmalloc_handler(void (*f)()))() {
            void (* old)() = __malloc_alloc_oom_handler;
            __malloc_alloc_oom_handler = f;
            return(old);
        }
    
    };

    // malloc_alloc out-of-memory handling
    // 初值定为0,有待客端设定
    template <int inst>
    void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler) () = 0;

    template<int inst>
    void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
        void (* my_malloc_handler) ();
        void * result;
        for (;;) {
            my_malloc_handler = __malloc_alloc_oom_handler; // 客户端不指定的话,直接抛出异常
            if (0 == my_malloc_handler) {
                throw std::bad_alloc();
            }
            (*my_malloc_handler)(); // 调用处理例程,企图释放内存
            result = malloc(n); // 再次尝试配置内存
            if (result) {
                return result;
            }
        }
    }

    template <int inst>
    void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
        void (* my_malloc_handler) ();
        void * result;
        for (;;) {
            my_malloc_handler = __malloc_alloc_oom_handler; // 客户端不指定的话,直接抛出异常
            if (0 == my_malloc_handler) {
                throw std::bad_alloc();
            }
            (*my_malloc_handler)(); // 调用处理例程,企图释放内存
            result = realloc(p, n); // 再次尝试配置内存
            if (result) {
                return result;
            }
        }
    }

    // 注意, 以下直接将参数 inst 指定为 0 (不影响)
    typedef __malloc_alloc_template<0> malloc_alloc;


    // 第二级配置器---------------------------------------------------------------
    // 内存池的设计
    enum {__ALIGN = 8}; // 小型区块的上调边界
    enum {__MAX_BYTES = 128}; //小型区块的上限
    enum {__NFREELISTS = __MAX_BYTES / __ALIGN}; // free-lists 个数

    template <bool threads, int inst>
    class __default_alloc_template {
    private:
        // ROUND_UP 将 bytes 上调至 8 的倍数
        static size_t ROUND_UP (size_t bytes) {
            return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1)); // 返回的数 二进制的后三位为0, 即为8的倍数
        }
        // free_list 节点结构
        union obj {
            union obj* free_list_link; // 相当于next
            char client_data[1]; // 相当于节点实值,指向实际区块的指针
        };
    private:
        static obj* volatile free_list[__NFREELISTS];
        // 根据区块大小, 决定使用第 n 号 free-list, n 从 0 开始
        static size_t FREELIST_INDEX(size_t bytes) {
            return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
        }
        // 返回一个大小为 n 的对象, 并可能加入大小为 n 的其他区块到 free list
        // 从内存池中索要区块到free list
        static void* refill(size_t n);
        // 配置一大块区间, 可容纳 nobjs 个大小为 "size" 的区块
        // 如果不够 nobjs 个空间, nobjs 可能会降低(默认为20)
        static char* chunk_alloc(size_t size, int& nobjs);

        // Chunk_allocation state
        static char* start_free; // 内存池起始位置. 只在chunk_alloc()中变化
        static char* end_free; // 内存池结束位置. 只在chunk_alloc()中变化
        static size_t heap_size;

    public:
        // 申请内存
        static void* allocate(size_t n);
        // 释放内存
        static void deallocate(void* p, size_t n);
        static void* reallocate(void* p, size_t old_sz, size_t new_sz);
    };

    // 初值定义
    template<bool threads, int inst>
    char* __default_alloc_template<threads, inst>::start_free = 0;
    template<bool threads, int inst>
    char* __default_alloc_template<threads, inst>::end_free = 0;
    template<bool threads, int inst>
    size_t __default_alloc_template<threads, inst>::heap_size = 0;

    template<bool threads, int inst>
    typename __default_alloc_template<threads, inst>::obj* volatile 
    __default_alloc_template<threads, inst>::free_list[__NFREELISTS] = 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // n > 0
    template<bool threads, int inst>
    void* __default_alloc_template<threads, inst>::allocate(size_t n) {
        obj* volatile *my_free_list; // 指向 obj* 的指针
        obj* result;
        // 大于128就使用第一级配置器
        if (n > (size_t) __MAX_BYTES) {
            return (malloc_alloc::allocate(n));
        }
        // 寻找16个free lists中适当的一个
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == 0) {
            // 没有找到可用的free list, 准备重新填充free list
            void* r = refill(ROUND_UP(n));
            return r;
        }
        // 调整free list
        *my_free_list = result->free_list_link;
        return result;
    }

    template<bool threads, int inst>
    void __default_alloc_template<threads, inst>::deallocate(void* p, size_t n) {
        obj* q = (obj*)p;
        obj* volatile * my_free_list;

        // 大于128就调用第一级配置器
        if (n > 128) {
            malloc_alloc::deallocate(p, n);
            return;
        }
        // 寻找相应的free list
        my_free_list = free_list + FREELIST_INDEX(n);
        // 回收
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }

    // n 为 8 的倍数
    template<bool threads, int inst>
    void* __default_alloc_template<threads, inst>::refill(size_t n) {
        int nobjs = 20; // 默认申请20个区块
        // 调用 chunk_alloc() 尝试从内存池中取得 nobjs 个区块作为free list的新节点
        char* chunk = chunk_alloc(n, nobjs);
        obj* volatile *my_free_list;
        obj* result;
        obj* current_obj, *next_obj;
        int i;

        // 只申请到 1 块区块, 直接给调用者用, 没有新的free list节点
        if (1 == nobjs) {
            return chunk;
        }
        // 准备调整 free list
        my_free_list = free_list + FREELIST_INDEX(n);

        // 在 chunk 空间内建立free list
        result = (obj*) chunk; // 第一块返回给调用者
        *my_free_list = next_obj = (obj*) (chunk + n); // my_free_list 指向链表的第一个节点
        // 串联起来
        for (i = 1; ; ++i) {
            current_obj = next_obj;
            next_obj = (obj*)((char*)(current_obj) + n);
            if (nobjs - 1 == i) {
                current_obj->free_list_link = 0; // 最后一个区块
                break;
            }
            else {
                current_obj->free_list_link = next_obj;
            }
        }
        return result;
    }

    // 从内存池中取出 nobjs 个区块
    template<bool threads, int inst>
    char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs) {
        char* result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free; // 内存池剩余空间

        // 内存池空间足够
        if (bytes_left >= total_bytes) {
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else if (bytes_left >= size) {
            // 内存池空间不够 nobjs 个, 但满足一个以上
            nobjs = bytes_left / size;
            total_bytes = nobjs * size;
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else {
            // 一个区块都不够
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4); // 字节大小为 2 倍所需 + 自增加量
            // 以下使内存池中剩余的字节数还有用
            if (bytes_left > 0) {
                // 寻找合适的free list
                obj* volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
                ((obj*)start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj*)start_free;
            }

            // 配置 heap 空间, 用来补充内存池
            start_free = (char*)malloc(bytes_to_get);
            if (0 == start_free) {
                // heap 空间不足, malloc 失败
                int i;
                obj* volatile *my_free_list, *p; // p 是指向 obj 的指针
                // 试着检查free list 数组中其他的free list是否还有剩余的块(且比需要的空间大)
                for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (0 != p) {
                        // 调整 free list 以释放未用区块
                        *my_free_list = p->free_list_link;
                        start_free = (char*)p;
                        end_free = start_free + i;
                        // 内存有了, 递归调用自己, 修正 nobjs
                        return (chunk_alloc(size, nobjs)); // 这里返回了函数就结束了, 不一定会把循环执行完
                    }
                }
                end_free = 0; // free list 数组中其他free list也没有剩余的内存了
                // 调用第一级配置器 (内存情况得到改善或抛出异常) 这里是抛出异常
                start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            // 内存申请到了, 递归调用自己, 修正 nobjs
            return (chunk_alloc(size, nobjs));
        }
    }

    // 别名
    typedef __default_alloc_template<false, 0> alloc;

    // 对外接口 默认使用第一配置器和第二配置器结合
    // Alloc 定为 alloc 即可. 
    // 第二级配置器在大于 128 bytes 时会使用第一级配置器
    template<class T, class Alloc>
    class simple_alloc {
    public:
        static T* allocate(size_t n) {
            return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T)); // 转换为 bytes
        }
        static T* allocate(void) {
            return (T*)Alloc::allocate(sizeof(T)); // 没有参数版本申请一个空间
        }
        static void deallocate(T* p, size_t n) {
            if (0 != n) {
                Alloc::deallocate(p, n * sizeof(T)); // 释放 n bytes 内存
            }
        }
        static void deallocate(T* p) {
            Alloc::deallocate(p, sizeof(T)); // 只释放当前指针的内存
        }
    };

}


#endif