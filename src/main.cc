// Copyright 2026 Takumi Sugimoto

#include <iostream>
#include "./ecs.h"
#include "./tcp.h"

int main() {
    std::cout << "start task" << std::endl;

    listen();
    run();

    std::cout << "complete task" << std::endl;
    return 0;
}
