cmake_minimum_required(VERSION 3.20)
project(entt_inmemorydb VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # ccls

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cc" "codegen/*.cc")
add_executable(entt_inmemorydb ${SOURCES})
target_link_libraries(entt_inmemorydb PRIVATE protobuf::libprotobuf)
target_link_libraries(entt_inmemorydb PRIVATE gRPC::grpc++)
target_include_directories(entt_inmemorydb PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(entt_inmemorydb PRIVATE ${PROJECT_SOURCE_DIR}/include)
