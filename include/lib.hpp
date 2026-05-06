#pragma once

#include <iostream>
#include <entt/entt.hpp>

struct Position {
    float x;
    float y;
};

struct Cell {
    int x;
    int y;
};

int div_euclid(float n, int d) {
    return std::floor(n / d);
}

int run() {
    entt::registry reg;
    entt::reactive_mixin<entt::storage<void>> storage;

    storage.bind(reg);
    storage.on_construct<Position>();
    storage.on_update<Position>();

    const int range = 256;
    for (int y = -range; y < range; ++y) {
        for (int x = -range; x < range; ++x) {
            const auto e = reg.create();
            reg.emplace<Position>(e, x * 1.0f, y * 1.0f);
        }
    }

    for (auto e : storage) {
        auto& p = reg.get<Position>(e);
        reg.emplace_or_replace<Cell>(e, div_euclid(p.x, 16), div_euclid(p.y, 16));
    }

    std::cout << "complete task" << std::endl;
    return 0;
}
