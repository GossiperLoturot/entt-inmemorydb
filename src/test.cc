// Copyright 2026 Takumi Sugimoto

#include <gtest/gtest.h>

#include "include/ecs.h"

TEST(ECS, DryRun) {
    EXPECT_EQ(calc(1, 2), 3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
