#include <iostream>
#include <grpcpp/grpcpp.h>
#include "model.grpc.pb.h"

class GreeterServer final : public greet::Greeter::Service {
public:
    grpc::Status SayHello(
        grpc::ServerContext* ctx,
        const greet::HelloRequest* req,
        greet::HelloReply* rep
    ) override {
        std::string prefix("Hello ");
        rep->set_message(prefix + req->name());
        return grpc::Status::OK;
    }
};
