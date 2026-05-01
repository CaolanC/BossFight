#include <gtest/gtest.h>

#include <SceneSnapshot.hpp>
#include <TestHelpers.hpp>

// Tests scene snapshot functions and its map. More details in docs

TEST(SceneSnapshotTests, NewSnapshotEmpty) {
    core::SceneSnapshot snapshot;

    EXPECT_TRUE(snapshot.getmap().empty());
}

TEST(SceneSnapshotTests, InsertAddsObject) {
    core::SceneSnapshot snapshot;

    auto obj = make_test_object();
    snapshot.insert(obj.objectID, obj);

    EXPECT_EQ(snapshot.getmap().size(), 1);
}

TEST(SceneSnapshotTests, FindObjectReturnsCorrectObject) {
    core::SceneSnapshot snapshot;

    auto obj = make_test_object();
    snapshot.insert(obj.objectID, obj);

    auto* result = snapshot.find("obj-1");

    ASSERT_NE(result, nullptr);

    EXPECT_EQ(result->name, "Object");
}

TEST(SceneSnapshotTests, FindMissingObjectReturnsNullptr) {
    core::SceneSnapshot snapshot;

    auto* result = snapshot.find("missing-object");

    EXPECT_EQ(result, nullptr);
}

TEST(SceneSnapshotTests, DeleteExistingObjectRemovesIt) {
    core::SceneSnapshot snapshot;

    auto obj = make_test_object();
    snapshot.insert(obj.objectID, obj);
    snapshot.delete_object("obj-1");

    EXPECT_EQ(snapshot.find("obj-1"), nullptr);
    EXPECT_TRUE(snapshot.getmap().empty());
}

TEST(SceneSnapshotTests, GetMapReturnsMapCopy) {
    core::SceneSnapshot snapshot;

    auto obj = make_test_object();
    snapshot.insert(obj.objectID, obj);

    std::unordered_map<std::string, core::SerializedObject> copy = snapshot.getmap();

    auto mapfind = copy.find("obj-1");

    auto result = &mapfind->second;
    auto* result2 = snapshot.find("obj-1");

    ASSERT_NE(result, nullptr);
    ASSERT_NE(result2, nullptr);

    EXPECT_EQ(result->objectID, result2->objectID);
    EXPECT_EQ(result->model_ref, obj.model_ref);
    EXPECT_EQ(result->name, "Object");
    EXPECT_EQ(result->model_path, obj.model_path);
    EXPECT_EQ(result->position, obj.position);
    EXPECT_EQ(result->rotation, obj.rotation);
    EXPECT_EQ(result->scale, obj.scale);
}

TEST(SceneSnapshotTests, DeleteMissingObjectDoesNotThrow) {
    core::SceneSnapshot snapshot;

    EXPECT_NO_THROW(snapshot.delete_object("missing-object"));
}

TEST(SceneSnapshotTests, InsertSameIdReplacesObject) {
    core::SceneSnapshot snapshot;

    auto first = make_test_object();
    auto second = make_test_object();
    second.name = "new_chair";

    snapshot.insert(first.objectID, first);
    snapshot.insert(second.objectID, second);

    auto* result = snapshot.find("obj-1");

    ASSERT_NE(result, nullptr);

    EXPECT_EQ(result->name, "new_chair");
    EXPECT_EQ(snapshot.getmap().size(), 1);
}

