/**
* @file timestamp.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-20-16-36
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef TIMESTAMP
#define TIMESTAMP

#include <chrono>
using namespace std::chrono;

class timestamp {
public:
    timestamp();
    ~timestamp();
    void update();
    double second();
    double millsecond();
    long long microsecond();
private:
    time_point<high_resolution_clock> begin_;
};

#endif // TIMESTAMP