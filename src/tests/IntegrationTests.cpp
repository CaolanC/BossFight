#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <TestHelpers.hpp>
#include <utils/assets/helpers.hpp>
#include <JSONHelper.hpp>
#include <SceneSnapshot.hpp>
#include <SerializedObject.hpp>
#include <SceneSerializer.hpp>
#include <nlohmann/json.hpp>
#include <spawn/Spawn.hpp>
#include <systems/Transform.hpp>
#include <component/Core.hpp>
#include <Session.hpp>
#include <SessionManager.hpp>
#include <NetBus.hpp>

#include "Client.hpp"
#include "Server.hpp"

// Multiple component integration tests. More info in testing docs

TEST(IntegrationTests, SpawnEntityGoesThroughECS) {
    entt::registry r;

    auto e = spawn::model(r, xg::newGuid(), xg::newGuid(), "models/chair/scene.gltf", glm::vec3(5, 0, 0), 2.0f, glm::quat(1, 0, 0, 0), "obj-1", "Object");

    systems::Transform(r);

    auto t = r.get<shared::component::transform>(e);

    EXPECT_FLOAT_EQ(t[3][0], 5.0f);
    EXPECT_FLOAT_EQ(t[0][0], 2.0f);
}

TEST(IntegrationTests, NetworkSnapshotSaveAndLoadAfterDeserialization) {
    core::SceneSnapshot snapshot1;

    auto chair = make_test_object();
    chair.name = "Chair";
    auto table = make_test_object();
    table.objectID = "obj-2";
    table.name = "Table";
    table.position = glm::vec3(10.0f, 2.0f, -5.0f);
    table.scale = 3.0f;

    snapshot1.insert(chair.objectID, chair);
    snapshot1.insert(table.objectID, table);

    auto snapshot_message = shared::JSONHelper::make_snapshot_message(snapshot1);
    auto message_json = nlohmann::json::parse(snapshot_message);

    core::SceneSnapshot received = shared::JSONHelper::deserialize_snapshot_string(message_json["payload"].dump());

    const std::string path = "integration_test_ID2.json";

    ASSERT_TRUE(core::SceneSerializer::save(received, path));

    core::SceneSnapshot loaded;
    ASSERT_TRUE(core::SceneSerializer::load_from_file(path, loaded));

    ASSERT_NE(loaded.find("obj-1"), nullptr);
    ASSERT_NE(loaded.find("obj-2"), nullptr);

    EXPECT_EQ(loaded.find("obj-1")->name, "Chair");
    EXPECT_EQ(loaded.find("obj-2")->name, "Table");
    EXPECT_EQ(loaded.find("obj-2")->position, table.position);
    EXPECT_FLOAT_EQ(loaded.find("obj-2")->scale, 3.0f);

    std::filesystem::remove(path);
}

TEST(IntegrationTests, ApplyUpdateMessagesToSession) {
    server::Session session;

    auto obj = make_test_object();
    obj.name = "Original";

    auto addmsg = shared::JSONHelper::make_update_message("add", obj);
    auto addjson = nlohmann::json::parse(addmsg);
    auto addobj = shared::JSONHelper::deserialize_object_string(addjson["payload"].dump());

    session.addSnapshot(addobj);

    ASSERT_NE(session.getSnapshot().find("obj-1"), nullptr);
    EXPECT_EQ(session.getSnapshot().find("obj-1")->name, "Original");

    obj.name = "Edited";
    obj.scale = 4.0f;

    auto editmsg = shared::JSONHelper::make_update_message("edit", obj);
    auto editjson = nlohmann::json::parse(editmsg);
    auto editobj = shared::JSONHelper::deserialize_object_string(editjson["payload"].dump());

    session.editSnapshot(editobj);

    ASSERT_NE(session.getSnapshot().find("obj-1"), nullptr);
    EXPECT_EQ(session.getSnapshot().find("obj-1")->name, "Edited");
    EXPECT_FLOAT_EQ(session.getSnapshot().find("obj-1")->scale, 4.0f);

    auto deletemsg = shared::JSONHelper::make_update_message("delete", obj);
    auto deletejson = nlohmann::json::parse(deletemsg);
    auto deleteobj = shared::JSONHelper::deserialize_object_string(deletejson["payload"].dump());

    session.deleteFromSnapshot(deleteobj);

    EXPECT_EQ(session.getSnapshot().find("obj-1"), nullptr);
}

TEST(IntegrationTests, SceneLoadFailsAssetMissing) {
    core::SceneSnapshot snapshot;

    auto obj = make_test_object();
    obj.model_path = "missing/file.gltf";

    snapshot.insert(obj.objectID, obj);

    const std::string path = "integration_scene.json";
    ASSERT_TRUE(core::SceneSerializer::save(snapshot, path));

    core::SceneSnapshot loaded;
    ASSERT_TRUE(core::SceneSerializer::load_from_file(path, loaded));

    EXPECT_FALSE(systems::Local_files_exist(loaded));

    std::filesystem::remove(path);
}

// Networking-related integration tests. More info in testing docs

TEST(IntegrationTests, SessionManagerMapsSessionToPort) {
    server::NetBus bus;
    server::SessionManager manager(bus);

    xg::Guid sid = manager.create();

    auto* session = manager.getSession(sid);
    ASSERT_NE(session, nullptr);

    int port = 30001;
    session->setPSPort(port);
    manager.setportID(port, sid);

    auto* by_port = manager.getSessionByPort(port);

    ASSERT_NE(by_port, nullptr);
    EXPECT_EQ(by_port->get_id(), sid);
    EXPECT_EQ(by_port->getPort(), port);
}

TEST(IntegrationTests, SessionsReuseByInactivePorts) {
    server::NetBus bus;
    server::SessionManager manager(bus);

    xg::Guid sid = manager.create();

    auto* session = manager.getSession(sid);
    ASSERT_NE(session, nullptr);

    session->setPSPort(30001);
    session->setActive(true);

    EXPECT_EQ(manager.find_free_port(), 30002);

    session->setActive(false);

    EXPECT_EQ(manager.find_free_port(), 30001);
}

TEST(IntegrationTests, ClientMakesSessionRequest) {
    server::Server server(32001);
    server.start();

    HttpResponse response;
    ASSERT_TRUE(wait_for_http_ok("http://127.0.0.1:32001/ping", response));

    client::Client client(
        "Test Client",
        true
    );

    int ws_port = 0;

    bool ok = client.request_create_session("http://127.0.0.1:32001", ws_port);

    EXPECT_TRUE(ok);
    EXPECT_EQ(ws_port, 30001);

    server.stop();
}