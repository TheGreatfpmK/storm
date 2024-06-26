#include "storm/settings/SettingsManager.h"
#include "test/storm_gtest.h"

int main(int argc, char **argv) {
    storm::settings::initializeAll("Storm-pomdp (Functional) Testing Suite", "test-pomdp");
    ::testing::InitGoogleTest(&argc, argv);
    storm::test::initialize(&argc, argv);
    return RUN_ALL_TESTS();
}
