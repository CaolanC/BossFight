
#include <gtest/gtest.h>
#include <JSONHelper.hpp>
#include <SceneSnapshot.hpp>
#include <SerializedObject.hpp>
#include <TestHelpers.hpp>
#include <nlohmann/json.hpp>

TEST(JSONHelperTests, SerializeObjectPreservesFields) {
    auto obj = make_test_object();

    auto json = shared::JSONHelper::serialize_object(obj);
    auto result = shared::JSONHelper::deserialize_object(json);

    EXPECT_EQ(result.objectID, "obj-1");
    EXPECT_EQ(result.name, "Object");
    EXPECT_EQ(result.model_path, "models/test/scene.gltf");
    EXPECT_EQ(result.model_ref, obj.model_ref);
    EXPECT_EQ(result.position, obj.position);
    EXPECT_EQ(result.rotation, obj.rotation);
    EXPECT_EQ(result.scale, 2.0f);
}

TEST(JSONHelperTests, MakeHandshakeType) {
    auto msg = shared::JSONHelper::make_handshake(xg::Guid(), true);
    auto j = nlohmann::json::parse(msg);

    EXPECT_EQ(j["type"], "handshake");
}

TEST(JSONHelperTests, MakeHandshakeStoresClientInfoHost) {
    auto guid = xg::Guid();
    auto msg = shared::JSONHelper::make_handshake(guid, true);
    auto j = nlohmann::json::parse(msg);

    EXPECT_EQ(j["payload"]["client_id"], guid.str());
    EXPECT_EQ(j["payload"]["role"], "host");
}

TEST(JSONHelperTests, MakeHandshakeStoresClientInfoGuest) {
    auto guid = xg::Guid();
    auto msg = shared::JSONHelper::make_handshake(guid, false);
    auto j = nlohmann::json::parse(msg);

    EXPECT_EQ(j["payload"]["client_id"], guid.str());
    EXPECT_EQ(j["payload"]["role"], "client");
}

TEST(JSONHelperTests, MakeHandshakeAckType) {
    auto msg = shared::JSONHelper::make_handshake_ack();
    auto j = nlohmann::json::parse(msg);

    EXPECT_EQ(j["type"], "handshake_ack");
}

TEST(JSONHelperTests, MakeUpdateInvalidType) {
    auto obj = make_test_object();

    auto msg = shared::JSONHelper::make_update_message("invalidtype", obj);

    EXPECT_EQ(msg, "Invalid");
}

TEST(JSONHelperTests, MakeUpdateTypes) {
    auto obj = make_test_object();

    auto addmsg = shared::JSONHelper::make_update_message("add", obj);
    auto editmsg = shared::JSONHelper::make_update_message("edit", obj);
    auto delmsg = shared::JSONHelper::make_update_message("delete", obj);

    auto jadd = nlohmann::json::parse(addmsg);
    auto jedit = nlohmann::json::parse(editmsg);
    auto jdel = nlohmann::json::parse(delmsg);

    EXPECT_EQ(jadd["type"], "update_add");
    EXPECT_EQ(jedit["type"], "update_edit");
    EXPECT_EQ(jdel["type"], "update_delete");
}

TEST(JSONHelperTests, MakeSnapshotType) {
    core::SceneSnapshot snapshot;
    auto obj = make_test_object();

    snapshot.insert(obj.objectID, obj);

    auto msg = shared::JSONHelper::make_snapshot_message(snapshot);
    auto json = nlohmann::json::parse(msg);

    EXPECT_EQ(json["type"], "snapshot");
}

TEST(JSONHelperTests, MakeSnapshotWithEmpty) {
    core::SceneSnapshot snapshot;

    auto msg = shared::JSONHelper::make_snapshot_message(snapshot);
    auto json = nlohmann::json::parse(msg);

    EXPECT_EQ(json["type"], "snapshot");

    ASSERT_TRUE(json.contains("payload"));
    ASSERT_TRUE(json["payload"].contains("objects"));

    EXPECT_TRUE(json["payload"]["objects"].empty());
}

TEST(JSONHelperTests, SnapshotRoundTrip) {
    core::SceneSnapshot snapshot;

    auto obj = make_test_object();
    snapshot.insert(obj.objectID, obj);
    auto msg = shared::JSONHelper::make_snapshot_message(snapshot);
    auto loaded = shared::JSONHelper::deserialize_snapshot(nlohmann::json::parse(msg)["payload"]);

    auto* result = loaded.find("obj-1");

    ASSERT_NE(result, nullptr);

    EXPECT_EQ(result->objectID, obj.objectID);
    EXPECT_EQ(result->model_ref, obj.model_ref);
    EXPECT_EQ(result->name, "Object");
    EXPECT_EQ(result->model_path, obj.model_path);
    EXPECT_EQ(result->position, obj.position);
    EXPECT_EQ(result->rotation, obj.rotation);
    EXPECT_EQ(result->scale, obj.scale);
}



