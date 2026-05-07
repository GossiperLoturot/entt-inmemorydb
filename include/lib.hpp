#pragma once

#include <iostream>
#include <zmq.hpp>
#include "simulation_state_generated.h"

int run() {
    std::cout << "start task" << std::endl;

    zmq::context_t ctx(1);
    zmq::socket_t socket(ctx, zmq::socket_type::rep);
    socket.bind("ipc://pipe.run");

    flatbuffers::FlatBufferBuilder builder(1024);
    while (true) {
        zmq::message_t recv_msg;
        auto recv = socket.recv(recv_msg, zmq::recv_flags::none);
        std::cout << "recv message" << std::endl;

        builder.Clear();
        std::vector<flatbuffers::Offset<EntityData>> entity_vec;
        for (int32_t y = -16; y <= 16; ++y) {
            for (int32_t x = -16; x <= 16; ++x) {
                entity_vec.push_back(CreateEntityData(builder, x * 1.0f, 0.0f, y * 1.0f));
            }
        }
        auto entities = builder.CreateVector(entity_vec);
        auto state = CreateSimulationState(builder, entities);
        builder.Finish(state);

        uint8_t* ptr = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        zmq::message_t send_msg(ptr, size);
        socket.send(send_msg, zmq::send_flags::none);
    }

    std::cout << "complete task" << std::endl;
    return 0;
}
