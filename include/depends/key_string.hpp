/**
* @file key_string.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-26-15-11
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_KEY_STRING_HPP_
#define NEMAUSA_IO_KEY_STRING_HPP_

#include <cstring>

namespace nemausa {
namespace io {

class key_string {
public:
    key_string(const char *str);
    void set(const char *str);
    const char * get();
    friend bool operator < (const key_string &left, const key_string &right);
private:
    const char *str_ = nullptr;
};

} // namespace nemausa
} // namespace io

#endif // NEMAUSA_IO_KEY_STRING_HPP_