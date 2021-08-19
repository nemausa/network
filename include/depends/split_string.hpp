/**
* @file split_string.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-21-22-03
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_SPLIT_STRING_HPP_
#define NEMAUSA_IO_SPLIT_STRING_HPP_

#include <string>

namespace nemausa {
namespace io {

class split_string {
public:
    void set(char *str);
    char * get(char end);
    char * get(const char *end);
private:
    char *str_ = nullptr;
    bool first_ = true;
};

} // nemaspace nemausa   
} // namespace io 

#endif // NEMAUSA_IO_SPLIT_STRING_HPP_