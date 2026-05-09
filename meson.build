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
  'third_party/entt/single_include',
  'third_party/asio/include'
)

# input dependencies.
deps = [
  dependency('flatbuffers')
]

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

# input proto files.
fbs_sources = gen.process([
  'schemas/model.fbs'
])

executable(
  'main',
  sources,
  fbs_sources,
  include_directories: headers,
  dependencies: deps
)
