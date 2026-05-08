#include <iostream>
#include <grpcpp/grpcpp.h>
#include "model.grpc.pb.h"

class GreeterClient {
public:
    GreeterClient(std::shared_ptr<grpc::Channel> channel)
        : stub(greet::Greeter::NewStub(channel))
    {}

    std::string SayHello(const std::string& user) {
        greet::HelloRequest request;
        request.set_name(user);

        greet::HelloReply reply;
        grpc::ClientContext context;
        grpc::Status status = stub->SayHello(&context, request, &reply);

        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "gRPC failed";
        }
    }

private:
    std::unique_ptr<greet::Greeter::Stub> stub;
};
