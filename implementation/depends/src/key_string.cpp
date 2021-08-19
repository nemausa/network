#include "depends/key_string.hpp"

namespace nemausa {
namespace io {

key_string::key_string(const char *str) {
    set(str);
}

void key_string::set(const char *str) {
    str_ = str;
}

const char * key_string::get() {
    return str_;
}

bool operator <(const key_string &left, const key_string &right) {
    return strcmp(left.str_, right.str_) < 0;
}

} // namespace nemausa
} // namespace io