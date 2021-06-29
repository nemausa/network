/**
* @file alloctor.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-26-23-15
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef ALLOCTOR
#define ALLOCTOR
#include <cstddef>

void *operator new(size_t size);
void  operator delete(void *p) noexcept;
void *operator new[](size_t size);
void  operator delete[](void *p) noexcept;
void *mem_alloc(size_t size);
void  mem_free(void *p);

#endif // ALLOCTOR