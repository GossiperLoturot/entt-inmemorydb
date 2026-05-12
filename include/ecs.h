// Copyright 2026 Takumi Sugimoto

#pragma once

#include <Eigen/Geometry>
#include <flatbuffers/flatbuffers.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <asio.hpp>
#include <entt/entt.hpp>
#include "./model_generated.h"

struct Type {
    int type_id;
};

struct Position {
    float x;
    float y;
    float z;
};

struct Rotation {
    float x;
    float y;
    float z;
    float w;
};

struct TimeCtx {
    float uptime;
};

void init_world(entt::registry &registry) {
    registry.ctx().emplace<TimeCtx>(0.0f);

    for (int y = -32; y <= 32; ++y) {
        for (int x = -32; x <= 32; ++x) {
            entt::entity entity = registry.create();
            registry.emplace<Type>(entity, 0);
            registry.emplace<Position>(entity, x * 1.0f, sinf(x * 0.1f + y * 0.1f), y * 1.0f);
            registry.emplace<Rotation>(entity, 0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}

void update_world(entt::registry &registry, float delta) {
    auto &time_ctx = registry.ctx().get<TimeCtx>();

    // alloc cache
    Eigen::AngleAxisf delta_q(delta, Eigen::Vector3f(0.0f, 1.0f, 0.0f));

    auto view = registry.view<Position, Rotation>();
    for (auto &&[entity, position, rotation] : view.each()) {
        // position update
        position.y = position.y + sin(time_ctx.uptime) * delta;
        registry.replace<Position>(entity, position);

        // rotation update
        auto q = Eigen::Quaternionf(rotation.w, rotation.x, rotation.y, rotation.z) * delta_q;
        rotation.x = q.x();
        rotation.y = q.y();
        rotation.z = q.z();
        rotation.w = q.w();
        registry.replace<Rotation>(entity, rotation);
    }

    // elapsed time
    time_ctx.uptime += delta;
}

struct ListenProactor {
    asio::io_context &io_ctx;
    asio::ip::tcp::acceptor acceptor;
    asio::ip::tcp::socket socket;
    uint32_t receive_buf;
    std::vector<uint8_t> send_buf;

    entt::reactive_mixin<entt::storage<void>> construct_storage;
    entt::reactive_mixin<entt::storage<void>> destroy_storage;
    entt::reactive_mixin<entt::storage<void>> update_storage;
    flatbuffers::FlatBufferBuilder builder;

    explicit ListenProactor(asio::io_context &io_ctx, const int16_t port, entt::registry& registry) :
        io_ctx(io_ctx),
        acceptor(io_ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
        socket(io_ctx),
        receive_buf(0),
        builder(1024) {
        construct_storage.bind(registry);
        construct_storage.on_construct<Type>();

        destroy_storage.bind(registry);
        destroy_storage.on_destroy<Type>();

        update_storage.bind(registry);
        update_storage.on_update<Position>();
        update_storage.on_update<Rotation>();
    }

    void start_accept() {
        acceptor.async_accept(
            socket,
            [this](const asio::error_code &e) { accept_handler(e); });
    }

    void accept_handler(const asio::error_code &e) {
        if (!e) {
            std::cout << "connection established." << std::endl;
            start_receive();
        } else {
            std::cout << "failed to establish connection. " << e.message() << std::endl;
            start_close();
        }
    }

    void start_receive() {
        asio::async_read(
            socket,
            asio::buffer(&receive_buf, sizeof(receive_buf)),
            [this](const asio::error_code &e, const size_t size) { receive_handler(e, size); });
    }

    void receive_handler(const asio::error_code &e, const size_t size) {
        if (!e) {
            std::cout << "receive data. " << ntohl(receive_buf) << std::endl;
            start_send();
        } else {
            std::cout << "failed to receive data. " << e.message() << std::endl;
            start_close();
        }
    }

    void start_send() {
        builder.Clear();

        std::vector<Model::CreateEntityCommand> create_entity_cmds_vec;
        for (auto &&[entity, type] : construct_storage.view<Type>().each()) {
            int id = static_cast<int>(entity);
            int type_id = type.type_id;
            auto create_entity_cmd = Model::CreateEntityCommand(id, type_id);
            create_entity_cmds_vec.push_back(create_entity_cmd);
        }
        auto create_entity_cmds = builder.CreateVectorOfStructs(create_entity_cmds_vec);
        construct_storage.clear();

        std::vector<Model::RemoveEntityCommand> remove_entity_cmds_vec;
        for (auto &&[entity, _] : destroy_storage.view<Type>().each()) {
            int id = static_cast<int>(entity);
            auto remove_entity_cmd = Model::RemoveEntityCommand(id);
            remove_entity_cmds_vec.push_back(remove_entity_cmd);
        }
        auto remove_entity_cmds = builder.CreateVectorOfStructs(remove_entity_cmds_vec);
        destroy_storage.clear();

        std::vector<Model::UpdatePositionCommand> update_position_cmds_vec;
        for (auto &&[entity, position] : update_storage.view<Position>().each()) {
            int id = static_cast<int>(entity);
            auto update_position_cmd = Model::UpdatePositionCommand(id, position.x, position.y, position.z);
            update_position_cmds_vec.push_back(update_position_cmd);
        }
        auto update_position_cmds = builder.CreateVectorOfStructs(update_position_cmds_vec);

        std::vector<Model::UpdateRotationCommand> update_rotation_cmds_vec;
        for (auto &&[entity, rotation] : update_storage.view<Rotation>().each()) {
            int id = static_cast<int>(entity);
            auto update_rotation_cmd = Model::UpdateRotationCommand(id, rotation.x, rotation.y, rotation.z, rotation.w);
            update_rotation_cmds_vec.push_back(update_rotation_cmd);
        }
        auto update_rotation_cmds = builder.CreateVectorOfStructs(update_rotation_cmds_vec);

        update_storage.clear();

        int version = 0;
        auto cmds = Model::CreateCommands(builder, version, create_entity_cmds, remove_entity_cmds, update_position_cmds, update_rotation_cmds);
        builder.Finish(cmds);

        const uint8_t* buf = builder.GetBufferPointer();
        uint32_t payload_size = builder.GetSize();

        uint32_t payload_size_n = htonl(payload_size);

        send_buf.resize(sizeof(uint32_t) + payload_size);
        std::memcpy(send_buf.data(), &payload_size_n, sizeof(uint32_t));
        std::memcpy(send_buf.data() + sizeof(uint32_t), buf, payload_size);

        asio::async_write(
            socket,
            asio::buffer(send_buf),
            [this](const asio::error_code &e, const size_t size) { send_handler(e, size); });
    }

    void send_handler(const asio::error_code &e, const size_t size) {
        if (!e) {
            std::cout << "sent data. " << size << " bytes." << std::endl;
            start_receive();
        } else {
            std::cout << "failed to send data. " << e.message() << std::endl;
            start_close();
        }
    }

    void start_close() {
        socket.close();
    }
};

void run() {
    asio::io_context io_ctx;

    entt::registry registry;

    ListenProactor listener(io_ctx, 12345, registry);
    listener.start_accept();

    init_world(registry);

    const std::chrono::duration<float> frame_time(1.0f / 60.0f);

    auto last_time = std::chrono::high_resolution_clock::now();
    while (true) {
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = current_time - last_time;
        last_time = current_time;

        update_world(registry, elapsed.count());
        io_ctx.poll();

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> load_time = end_time - current_time;
        if (load_time < frame_time) {
            std::this_thread::sleep_for(frame_time - load_time);
        }
    }
}
