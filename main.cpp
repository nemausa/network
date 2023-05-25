#include <iostream>
#include <glog/logging.h>

using namespace std;

int main() {
    // Initialize Google's logging library.
    google::InitGoogleLogging("Google.txt");
    FLAGS_log_dir = "./log";  
    LOG(INFO) << "Found " << 5 << " cooikes";
    google::ShutdownGoogleLogging();
    cout << "hello world" << endl;
    getchar();
}
