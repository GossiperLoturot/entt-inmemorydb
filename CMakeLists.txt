cmake_minimum_required(VERSION 3.20)
project(entt_inmemorydb VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # ccls

find_package(cppzmq)
find_package(flatbuffers)

# library
add_library(entt_inmemorydb INTERFACE)

target_include_directories(entt_inmemorydb INTERFACE include)
target_include_directories(entt_inmemorydb INTERFACE third_party/entt/single_include)
target_link_libraries(entt_inmemorydb INTERFACE cppzmq)
target_link_libraries(entt_inmemorydb INTERFACE flatbuffers)

add_executable(entt_inmemorydb_app src/main.cpp)
target_link_libraries(entt_inmemorydb_app PRIVATE entt_inmemorydb)
