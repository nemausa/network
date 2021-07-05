/**
* @file object_pool.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-28-22-49
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef OBJECT_POOL
#define OBJECT_POOL

#include <stdlib.h>
#include <assert.h>
#include <mutex>

#ifdef _DEBUG
    #ifndef xprintf
    #include <stdio.hpp>
    #define xprintf(fmt,...) \
	printf("%s(%d)-<%s>: "##fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__
    #endif
#else
    #ifndef xprintf
    #define xprintf(...) 
    #endif
#endif

template <typename T, size_t block_num>
class object_pool {
public:
    object_pool() {
        init();
    }

    ~object_pool() {
        if (address_)
            delete[] address_;
    }

    void free_mem(void *p) {
        node *block = (node*)((char*)p - sizeof(node));
        xprintf("%llx, id=%d\n", block, block->id_);
        assert(1 == block->ref_);
        if (block->in_pool_) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (--block->ref_ != 0) {
                return;
            }
            block->next_ = head_;
            head_ = block;
        } else {
            if (--block->ref_ != 0) {
                return;
            }
            delete[] block;
        }
    }

    void* alloc_mem(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        node *p = nullptr;
        if (nullptr == head_) {
            p = (node*)new char[sizeof(T) +sizeof(node)];
            p->in_pool_ = false;
            p->id_ = -1;
            p->ref_ = 1;
            p->next_ = nullptr;
        } else {
            p = head_;
            head_ = head_->next_;
            assert(0 == p->ref_);
            p->ref_ = 1;
        }
        xprintf("%llx, id=%d, size=%d\n", p, p->id_, size);
        return ((char*)p + sizeof(node));
    }

private:
    void init() {
        assert(nullptr == address_);
        if (address_) {
            return;
        }

        size_t block_size = sizeof(T) +sizeof(node);
        size_t pool_size = block_num *  block_size;
        address_ = new char[pool_size];
        head_ = (node*)address_;
        head_->in_pool_ = true;
        head_->id_ = 0;
        head_->ref_ = 0;
        head_->next_ = nullptr;

        node *temp1 = head_;
        for (size_t n = 1; n < block_num; n++) {
            node *temp2 = (node*)(address_ + (n * block_size));
            temp2->in_pool_ = true;
            temp2->id_ = n;
            temp2->ref_ = 0;
            temp2->next_ = nullptr;
            temp1->next_ = temp2;
            temp1 = temp2;
        }
    }

private:
    class node {
    public:
        node *next_;
        int id_;
        char ref_;
        bool in_pool_;
    private:
        char c1;
        char c2;
    };
    node *head_;
    char *address_;
    std::mutex mutex_;
};

template<typename T, size_t block_num>
class pool_base {
public:
    void *operator new(size_t size) {
        return pool().alloc_mem(size);
    }

    void operator delete(void *p) {
        pool().free_mem(p);
    }

    template<typename ...Args>
    static T *careteObject(Args ... args) {
        T *obj = new T(args...);
        return obj;
    }

    static void destory(T *obj) {
        delete obj;
    }
private:
    typedef object_pool<T, block_num> type_pool;
    static type_pool &pool() {
        static type_pool instance;
        return instance;
    }
};

#endif // OBJECT_POOL