#include <iostream>
#include <grpcpp/grpcpp.h>
#include "greeter.h"

int main() {
    std::cout << "start task" << std::endl;

    GreeterClient greeter(grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials()));
    std::cout << greeter.SayHello("world") << std::endl;

    std::cout << "complete task" << std::endl;
    return 0;
}
