// Copyright 2026 Takumi Sugimoto
//
#pragma once

#include <Eigen/Geometry>
#include <flatbuffers/flatbuffers.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
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

struct SyncAttachment {
    entt::reactive_mixin<entt::storage<void>> construct_storage;
    entt::reactive_mixin<entt::storage<void>> destroy_storage;
    entt::reactive_mixin<entt::storage<void>> update_storage;

    explicit SyncAttachment(entt::registry &registry) {
        construct_storage.bind(registry);
        construct_storage.on_construct<Type>();

        destroy_storage.bind(registry);
        destroy_storage.on_destroy<Type>();

        update_storage.bind(registry);
        update_storage
            .on_update<Position>()
            .on_update<Rotation>();
    }

    void sink() {
        flatbuffers::FlatBufferBuilder builder(1024);

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

        int size = builder.GetSize();
        const uint8_t* buf = builder.GetBufferPointer();
        std::cout << "size: " << size <<  ", ptr: " << buf << std::endl;
    }
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
        position.z = position.z + sin(time_ctx.uptime);
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

void run() {
    entt::registry registry;

    auto sync = SyncAttachment(registry);
    init_world(registry);

    const std::chrono::duration<float> frame_time(1.0f / 60.0f);

    auto last_time = std::chrono::high_resolution_clock::now();
    while (true) {
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = current_time - last_time;
        last_time = current_time;

        update_world(registry, elapsed.count());
        sync.sink();

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> load_time = end_time - current_time;
        if (load_time < frame_time) {
            std::this_thread::sleep_for(frame_time - load_time);
        }
    }
}
