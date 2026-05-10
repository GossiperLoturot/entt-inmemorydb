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
        std::vector<flatbuffers::Offset<Model::CreateCommand>> create_cmds_vec{};
        std::vector<flatbuffers::Offset<Model::UpdateCommand>> update_cmds_vec{};
        std::vector<flatbuffers::Offset<Model::RemoveCommand>> remove_cmds_vec{};
        for (int y = -32; y <= 32; ++y) {
            for (int x = -32; x <= 32; ++x) {
                int id = x + y * 32;
                int type_id = 0;
                auto position = Model::Vec3{x * 1.0f, 0.0f, y * 1.0f};
                auto rotation = Model::Vec4{0.0f, 0.0f, 0.0f, 0.0f};
                auto create_cmd = Model::CreateCreateCommand(builder, id, type_id, &position, &rotation);
                create_cmds_vec.push_back(create_cmd);
            }
        }
        int version = 0;
        auto create_cmds = builder.CreateVector(create_cmds_vec);
        auto update_cmds = builder.CreateVector(update_cmds_vec);
        auto remove_cmds = builder.CreateVector(remove_cmds_vec);
        auto cmds = Model::CreateCommands(builder, version, create_cmds, update_cmds, remove_cmds);
        builder.Finish(cmds);

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
