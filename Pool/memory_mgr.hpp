/**
* @file memory_mgr.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-26-23-20
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef MEMORY_MGR
#define MEMORY_MGR

#include <stdlib.h>
#include <assert.h>
#include <mutex>

#ifdef _DEBUG
#include <stdio.hpp>
    // #define xprintf(...) printf(__VA_ARGS__)
    #define xprintf(fmt,...) \
	printf("%s(%d)-<%s>: "##fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define xprintf(...) 
#endif

#define MAX_MEMORY_SIZE     1024

class memory_alloc;

class memory_block {
public:
    memory_alloc *alloc_;
    memory_block *next_;
public:
    // 内存块编号
    int id_;
    // 引用次数
    int ref_;
    // 是否在内存池中
    int in_pool_;
private:
    // 预留
    char c1;
    char c2;
    char c3;     
};

class memory_alloc {
public:
    memory_alloc() {
        buf_ = nullptr;
        head_ = nullptr;
        size_ = 0;
        block_num_ = 0;
        xprintf("memory alloc");
    }

    ~memory_alloc() {
        if (buf_)
            free(buf_);
    }

    void *alloc_mem(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!buf_) {
            init();
        }

        memory_block *p = nullptr;
        if (nullptr == head_) {
            p = (memory_block*)malloc(size + sizeof(memory_block));
            p->in_pool_ = false;
            p->id_ = -1;
            p->ref_ = 1;
            p->alloc_ = nullptr;
            p->next_ = nullptr;
        } else {
            p = head_;
            head_ = head_->next_;
            p->ref_ = 0;
        }
        xprintf("alloc:%llx, id=%d, size=%d\n", p, p->id_, size);
        return ((char*)p + sizeof(memory_block));
    }
    
    void free_mem(void *p) {
        memory_block *block = (memory_block*)((char*)p - sizeof(memory_block));
        if (block->in_pool_) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (--block->ref_ != 0){
                return;
            }
            block->next_ = head_;
            head_ = block;
        } else {
            if (--block->ref_ != 0) {
                return;
            }
            free(block);
        }
    }

    void init() {
        xprintf("size=%d, blcok_size=%d\n", size_, block_num_);
        assert(nullptr == buf_);
        if (buf_)
            return;
        size_t block_size = size_ + sizeof(memory_block);
        size_t buf_size = block_size * block_num_;
        buf_ = (char*)malloc(buf_size);

        head_ = (memory_block*)buf_;
        head_->in_pool_ = true;
        head_->id_ = 0;
        head_->ref_ = 0;
        head_->alloc_ = this;
        head_->next_ = nullptr;
        memory_block *temp1 = head_;

        for (size_t n = 1; n < block_num_; n++) {
            memory_block *temp2 = (memory_block*)(buf_ + (n * block_size));
            temp2->in_pool_ = true;
            temp2->id_ = n;
            temp2->ref_ = 0;
            temp2->alloc_ = this;
            temp2->next_ = nullptr;
            temp1->next_ = temp2;
            temp1 = temp2;
        }

    }

protected:
    char *buf_;
    memory_block *head_;
    size_t size_;
    size_t block_num_;
    std::mutex mutex_;
};

template<size_t size, size_t block_num>
class memory_alloctor : public memory_alloc {
public:
    memory_alloctor() {
        const size_t n = sizeof(void*);
        size_ = (size / n) * n + (size % n ? n : 0);
        block_num_ = block_num;
    }
};

class memory_mgr {
public:
    static memory_mgr& instance() {
        static memory_mgr mgr;
        return mgr;
    }

    void *alloc_mem(size_t size) {
        if (size <= MAX_MEMORY_SIZE) {
            return allocs_[size]->alloc_mem(size);
        } else {
            memory_block *p = (memory_block*)malloc(size + sizeof(memory_block));
            p->in_pool_ = false;
            p->id_ = -1;
            p->ref_ = 1;
            p->alloc_ = nullptr;
            p->next_ = nullptr;
            xprintf("%llx, id=%d, size=%d\n", p, p->id_, size);
            return ((char*)p +sizeof(memory_block));
        }
    }

    void free_mem(void *p) {
        memory_block *block = (memory_block*)((char*)p - sizeof(memory_block));
        if (block->in_pool_) {
            block->alloc_->free_mem(p);
        } else {
            if (--block->ref_ == 0) {
                free(block);
            }
        }
    }

    void add_ref(void *p) {
        memory_block *block = (memory_block*)((char*)p -sizeof(memory_block));
        ++block;
    }

private:
    memory_mgr() {
        init_alloc(0, 64, &mem64_);
        init_alloc(65, 128, &mem128_);
        init_alloc(129, 256, &mem256_);
        init_alloc(257, 512, &mem512_);
        init_alloc(513, 1204, &mem1024_);
        xprintf();
    }

    void init_alloc(int begin, int end, memory_alloc *p) {
        for (int n = begin; n <= end; n++){
            allocs_[n] = p;
        }
    }
private:
    memory_alloctor<64, 100000> mem64_;
    memory_alloctor<128, 100000> mem128_;
    memory_alloctor<256, 100000> mem256_;
    memory_alloctor<512, 100000> mem512_;
    memory_alloctor<1024, 100000> mem1024_;
    memory_alloc *allocs_[MAX_MEMORY_SIZE + 1];
};

#endif // MEMORY_MGR