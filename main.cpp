#include <iostream>
#include <csignal>
#include <string>
#include <unistd.h>
#include <atomic>
#include "my_shared_ptr.h"

static std::atomic<bool> quit;

// 捕捉信号，用于程序的优雅退出
static void signalHandler(int signum) {
    quit = true;
}

static void test() {
    MyShared_ptr<char> char1(nullptr);
    MyShared_ptr<int> int1(new(std::nothrow) int(10));
    std::cout << "int1=" << *int1 << std::endl;
    *int1 = 100 ;
    std::cout << "int1=" << *int1 << std::endl;
}

int main(int argc, char** argv) {
    quit = false;
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    test();

    while (!quit) {
        usleep(200000);
    }

    std::cout << "\nprogram going to quit\n";

    return 0;
}
