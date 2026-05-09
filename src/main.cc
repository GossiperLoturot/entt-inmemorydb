#include <iostream>
#include <grpcpp/grpcpp.h>
#include "greeter.h"

int main() {
    std::cout << "start task" << std::endl;

    std::string server_addr = "0.0.0.0:50051";

    GreeterServer service;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> handle(builder.BuildAndStart());
    std::cout << "server listening on " + server_addr << std::endl;
    handle->Wait();

    std::cout << "complete task" << std::endl;
    return 0;
}
