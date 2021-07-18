#include "alloctor.hpp"
#include "memory_mgr.hpp"

void *operator new(size_t size) {
    return memory_mgr::instance().alloc_mem(size);
}

void operator delete(void *p) {
    memory_mgr::instance().free_mem(p);
}

void *operator new[](size_t size) {
    return memory_mgr::instance().alloc_mem(size);
}

void operator delete[](void *p) {
    memory_mgr::instance().free_mem(p);
}

void *mem_alloc(size_t size) {
    return malloc(size);
}

void mem_free(void *p) {
    free(p);
}
