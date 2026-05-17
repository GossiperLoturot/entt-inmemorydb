// Copyright 2026 Takumi Sugimoto

#pragma once

#include <flatbuffers/flatbuffers.h>
#include <vector>
#include <asio.hpp>
#include <entt/entt.hpp>

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

void init_world(entt::registry &registry);

void update_world(entt::registry &registry, float delta);

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

    explicit ListenProactor(asio::io_context &io_ctx, const int16_t port, entt::registry& registry);

    void start_accept();

    void accept_handler(const asio::error_code &e);

    void start_receive();

    void receive_handler(const asio::error_code &e, const size_t size);

    void start_send();

    void send_handler(const asio::error_code &e, const size_t size);

    void start_close();
};

void run();
