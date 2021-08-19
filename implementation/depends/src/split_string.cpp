#include <cstring>
#include "depends/split_string.hpp"

namespace nemausa {
namespace io {

void split_string::set(char *str) {
    str_ = str;
    first_ = true;
}

char * split_string::get(char end) {
    if (!str_)
        return nullptr;

    char *temp = strchr(str_, end);
    if (!temp) {
        if (first_) {
            first_ = false;
            return str_;
        }
        return nullptr;
    }
    temp[0] = '\0';
    char *ret = str_;
    str_ = temp + 1;
    return ret;
}

char * split_string::get(const char *end) {
    if (!str_ || !end)
        return nullptr;
    
    char *temp = strstr(str_, end);
    if (!temp) {
        if (first_) {
            first_ = false;
            return str_;
        }
        return nullptr;
    }
    temp[0] = '\0';
    char *ret = str_;
    str_ = temp + strlen(end);

    return ret;
}

}
}