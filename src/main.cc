#include <iostream>
#include "ecs.h"

int main() {
    std::cout << "start task" << std::endl;

    run();

    std::cout << "complete task" << std::endl;
    return 0;
}
