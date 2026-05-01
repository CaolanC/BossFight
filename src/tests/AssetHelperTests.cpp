#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <TestHelpers.hpp>
#include <utils/assets/helpers.hpp>

// Tests some asset helper functions. More details in docs

TEST(AssetHelperTests, GetFilepathPrefixesAssetDirectory) {
    auto result = utils::assets::get_asset("models/test/scene.gltf");

    EXPECT_NE(result.find(SLJA_ASSETS_DIR), std::string::npos);
    EXPECT_NE(result.find("models/test/scene.gltf"), std::string::npos);
}

TEST(AssetHelperTests, GetSaveFilepathPrefixesSaveDirectory) {
    auto result = utils::assets::get_filepath("test_scene.json");

    EXPECT_NE(result.find(SLJA_SAVES_DIR), std::string::npos);
    EXPECT_NE(result.find("test_scene.json"), std::string::npos);
}

TEST(AssetPathTests, MissingAssetReturnsFalse) {
    EXPECT_FALSE(utils::assets::asset_exists("missing_asset.txt"));
}

TEST(AssetPathTests, AssetPathReturnsTrue) {
    std::filesystem::create_directories(std::string(SLJA_ASSETS_DIR) + "unit_tests");

    const std::string path1 = "unit_tests/existing_a.gltf";
    const std::string path2 = "unit_tests/existing_b.gltf";

    {
        std::ofstream(std::string(SLJA_ASSETS_DIR) + path1) << "{}";
        std::ofstream(std::string(SLJA_ASSETS_DIR) + path2) << "{}";
    }

    EXPECT_TRUE(utils::assets::asset_exists(path1));
    EXPECT_TRUE(utils::assets::asset_exists(path2));

    std::filesystem::remove(std::string(SLJA_ASSETS_DIR) + path1);
    std::filesystem::remove(std::string(SLJA_ASSETS_DIR) + path2);
}

TEST(AssetPathTests, SavePathReturnsTrue) {
    std::filesystem::create_directories(std::string(SLJA_SAVES_DIR) + "unit_tests");

    const std::string path1 = "unit_tests/save1.json";
    const std::string path2 = "unit_tests/save2.json";

    {
        std::ofstream(std::string(SLJA_SAVES_DIR) + path1) << "{}";
        std::ofstream(std::string(SLJA_SAVES_DIR) + path2) << "{}";
    }

    EXPECT_TRUE(utils::assets::filepath_exists(path1));
    EXPECT_TRUE(utils::assets::filepath_exists(path2));

    std::filesystem::remove(std::string(SLJA_SAVES_DIR) + path1);
    std::filesystem::remove(std::string(SLJA_SAVES_DIR) + path2);
}