#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>

#include "alloctor.hpp"
#include "memory_mgr.hpp"
#include "object_pool.hpp"

const int thread_count = 8;
const int m_count = 100000;
const int n_count = m_count / thread_count;

class ClassA : public pool_base<ClassA, 1>
{
public:
	ClassA(int n)
	{
		num = n;
		printf("ClassA\n");
	}

	~ClassA()
	{
		printf("~ClassA\n");
	}
public:
	int num = 0;
};

void work(int index) {
    char *data[n_count];
    for (size_t i = 0; i < n_count; i++) {
        data[i] = new char[(rand() % 128) +1];
    }
    for (size_t i=0; i < n_count; i++) {
        delete[] data[i];
    }
}

int main() {
    // std::thread t[thread_count];
    // for (int i = 0; i < thread_count; i++) {
    //     t[i] = std::thread(work, i);
    // }
    // for (int i = 0; i < thread_count; i++) {
    //     t[i].join();
    // }

    ClassA *data[10];
    for (size_t i = 0; i < 10; i++) {
        data[i] = ClassA::careteObject(6);
    }
    for (size_t i = 0; i < 10; i++) {
        ClassA::destory(data[i]);
    }
    xprintf();
    return 0;
}