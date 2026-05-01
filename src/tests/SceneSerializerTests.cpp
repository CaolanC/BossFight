#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <SceneSerializer.hpp>
#include <SceneSnapshot.hpp>
#include <TestHelpers.hpp>

// Tests scene serialization functions (from files, etc). More details in docs

TEST(SceneSerializerTests, InvalidPathReturnsFalse) {
    core::SceneSnapshot snapshot;

    bool ok = core::SceneSerializer::save(snapshot, "/invalid/path/test_scene.json");

    EXPECT_FALSE(ok);
}

TEST(SceneSerializerTests, SaveCreatesReadableFile) {
    core::SceneSnapshot snapshot;

    auto obj = make_test_object();
    snapshot.insert(obj.objectID, obj);

    const std::string path = "testpath.json";

    ASSERT_TRUE(core::SceneSerializer::save(snapshot, path));
    EXPECT_TRUE(std::filesystem::exists("testpath.json"));

    std::filesystem::remove(path);
}

TEST(SceneSerializerTests, LoadReturnsFalseMissingFile) {
    core::SceneSnapshot snapshot;

    bool ok = core::SceneSerializer::load_from_file("missing_scene_file.json", snapshot);

    EXPECT_FALSE(ok);
}

TEST(SceneSerializerTests, LoadReturnsFalseForJsonWithoutObjects) {
    const std::string path = "test_invalid_scene.json";

    {
        std::ofstream file(path);
        file << R"({"not_objects": []})";
    }

    core::SceneSnapshot snapshot;

    bool ok = core::SceneSerializer::load_from_file(path, snapshot);

    EXPECT_FALSE(ok);

    std::filesystem::remove(path);
}

TEST(SceneSerializerTests, SaveThenLoadPreservesSnapshotObject) {
    core::SceneSnapshot snapshot;

    auto obj = make_test_object();
    snapshot.insert(obj.objectID, obj);
    const std::string path = "test_scene_roundtrip.json";

    ASSERT_TRUE(core::SceneSerializer::save(snapshot, path));

    core::SceneSnapshot loaded;

    ASSERT_TRUE(core::SceneSerializer::load_from_file(path, loaded));

    auto* result = loaded.find("obj-1");

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->objectID, obj.objectID);
    EXPECT_EQ(result->model_path, obj.model_path);
    EXPECT_EQ(result->name, obj.name);
    EXPECT_EQ(result->model_ref, obj.model_ref);
    EXPECT_EQ(result->position, obj.position);
    EXPECT_EQ(result->rotation, obj.rotation);
    EXPECT_FLOAT_EQ(result->scale, obj.scale);

    std::filesystem::remove(path);
}