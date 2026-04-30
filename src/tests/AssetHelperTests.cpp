#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include <utils/assets/helpers.hpp>

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