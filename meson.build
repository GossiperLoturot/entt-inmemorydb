project(
  'entt-inmemory', 'cpp',
  version : '0.1.0',
  default_options : ['cpp_std=c++17']
)

# input source files.
sources = [
  'src/main.cc',
]

# project dir and build dir is auto included.
headers = include_directories(
  'include',
  'third_party/entt/single_include'
)

# input dependencies.
deps = [
  dependency('protobuf'),
  dependency('grpc++')
]

protoc = find_program('protoc')
grpc_cpp_plugin = find_program('grpc_cpp_plugin')
gen = generator(
  protoc,
  output: [
    '@BASENAME@.pb.cc', 
    '@BASENAME@.pb.h',
    '@BASENAME@.grpc.pb.cc', 
    '@BASENAME@.grpc.pb.h'
  ],
  arguments: [
    '--proto_path=@CURRENT_SOURCE_DIR@/protos',
    '--cpp_out=@BUILD_DIR@',
    '--grpc_out=@BUILD_DIR@',
    '--plugin=protoc-gen-grpc=' + grpc_cpp_plugin.full_path(),
    '@INPUT@'
  ]
)

# input proto files.
proto_sources = gen.process([
  'protos/model.proto'
])

executable(
  'main',
  sources,
  proto_sources,
  include_directories: headers,
  dependencies: deps
)
