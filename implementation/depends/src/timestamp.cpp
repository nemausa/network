#include "depends/timestamp.hpp"

namespace nemausa {
namespace io {

timestamp::timestamp() {
    update();
}

timestamp::~timestamp() {

}

void timestamp::update() {
    begin_ = high_resolution_clock::now();
}

double timestamp::second() {
    return microsecond() * 0.000001;
}

double timestamp::millsecond() {
    return microsecond()* 0.001;
}

long long timestamp::microsecond() {
    return duration_cast<microseconds>(high_resolution_clock::now() - begin_).count();
}

} // namespace io 
} // namespace nemausa