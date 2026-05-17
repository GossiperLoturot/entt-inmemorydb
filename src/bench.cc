// Copyright 2026 Takumi Sugimoto

#include <benchmark/benchmark.h>

#include "include/ecs.h"

static void benchmark_example(benchmark::State& state) {
    for (auto _ : state) {
        calc(1, 2);
    }
}

BENCHMARK(benchmark_example);
BENCHMARK_MAIN();
