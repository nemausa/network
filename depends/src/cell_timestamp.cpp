#include "cell_timestamp.hpp"

cell_timestamp::cell_timestamp() {
    update();
}

cell_timestamp::~cell_timestamp() {

}

void cell_timestamp::update() {
    begin_ = high_resolution_clock::now();
}

double cell_timestamp::second() {
    return microsecond() * 0.000001;
}

double cell_timestamp::millsecond() {
    return microsecond()* 0.001;
}

long long cell_timestamp::microsecond() {
    return duration_cast<microseconds>(high_resolution_clock::now() - begin_).count();
}