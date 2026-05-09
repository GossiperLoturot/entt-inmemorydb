#pragma once

#include <iostream>
#include <asio.hpp>
#include <flatbuffers/flatbuffers.h>
#include "model_generated.h"

int run() {
    try {
        asio::io_context ctx{};
        asio::ip::tcp::acceptor acceptor{ctx, asio::ip::tcp::endpoint{asio::ip::make_address("0.0.0.0"), 12345}};
        std::cout << "server is waiting for connection." << std::endl;

        asio::ip::tcp::socket socket{ctx};
        acceptor.accept(socket);
        std::cout << "client connected." << std::endl;

        flatbuffers::FlatBufferBuilder builder{1024};
        int id = 10;
        auto vec3 = Model::Vec3{1.0f, 2.0f, 3.0f};
        auto name = builder.CreateString("zero");
        auto entity = Model::CreateEntity(builder, id, &vec3, name);
        builder.Finish(entity);

        uint8_t* buffer = builder.GetBufferPointer();
        uint32_t size = builder.GetSize();
        std::cout << "buffer size : " << size << std::endl;
        
        std::vector<asio::const_buffer> buffers{};
        buffers.push_back(asio::buffer(&size, sizeof(size)));
        buffers.push_back(asio::buffer(buffer, size));
        asio::write(socket, buffers);

    } catch (const std::exception& e) {
        std::cerr << "server exception : " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
