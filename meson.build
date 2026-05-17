project(
  'entt-inmemory', 'cpp',
  version : '0.1.0',
  default_options : ['cpp_std=c++17']
)

# input dependencies.
deps = [
  dependency('asio'),
  dependency('eigen3'),
  dependency('flatbuffers'),
]

# project dir and build dir is auto included.
headers = include_directories('third_party/entt/single_include')

flatc = find_program('flatc')
gen = generator(
  flatc,
  output: [
    '@BASENAME@_generated.h', 
  ],
  arguments: [
    '--cpp',
    '-o',
    '@BUILD_DIR@',
    '@INPUT@'
  ]
)
fbs_sources = gen.process(['schemas/model.fbs'])

# build library.
lib = static_library(
  'core',
  ['src/ecs.cc'] + fbs_sources,
  include_directories: headers,
  dependencies: deps
)

# build target.
exe = executable(
  'main',
  ['src/main.cc'],
  include_directories: headers,
  dependencies: deps,
  link_with: lib
)
