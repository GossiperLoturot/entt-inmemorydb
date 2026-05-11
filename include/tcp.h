// Copyright 2026 Takumi Sugimoto

#pragma once

#include <iostream>
#include <vector>
#include <asio.hpp>

struct ListenProactor {
    asio::io_context &io_ctx;
    asio::ip::tcp::acceptor acceptor;
    asio::ip::tcp::socket socket;
    std::vector<uint8_t> buf;

    explicit ListenProactor(asio::io_context &io_ctx, const int16_t port):
        io_ctx(io_ctx),
        acceptor(io_ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
        socket(io_ctx)
    {}

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
        buf.clear();
        buf.resize(1024);
        socket.async_receive(
            asio::buffer(buf),
            [this](const asio::error_code &e, const size_t size) { receive_handler(e, size); });
    }

    void receive_handler(const asio::error_code &e, const size_t size) {
        if (!e) {
            std::cout << "receive data. " << buf.size() << std::endl;
            start_close();
        } else {
            std::cout << "failed to receive data. " << e.message() << std::endl;
            start_close();
        }
    }

    void start_close() {
        socket.close();
        start_accept();
    }
};

void listen() {
    asio::io_context io_ctx;
    asio::signal_set sigh(io_ctx, SIGINT);

    ListenProactor listener(io_ctx, 12345);
    listener.start_accept();

    auto guard = asio::make_work_guard(io_ctx);
    sigh.async_wait([&guard](auto e, auto n) { guard.reset(); });
    auto thread = std::async(
        std::launch::async,
        [&io_ctx]() { io_ctx.run(); });
    thread.get();
}
