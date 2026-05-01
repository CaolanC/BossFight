#include <gtest/gtest.h>

#include <Session.hpp>
#include <SessionManager.hpp>
#include <NetBus.hpp>

#include <TestHelpers.hpp>

// Tests some methods related to sessions in server. More details in docs

TEST(SessionTests, SessionStartsInactive) {
    server::Session session;

    EXPECT_FALSE(session.isActive());
}

TEST(SessionTests, SessionResetClearsData) {
    server::Session session;

    auto obj = make_test_object();

    session.setActive(true);
    session.setJoinable(true);
    session.getSnapshot().insert(obj.objectID, obj);

    session.reset();

    EXPECT_FALSE(session.isGuestJoinable());
    EXPECT_TRUE(session.getSnapshot().getmap().empty());
}

TEST(SessionTests, ResetClearsMultipleSnapshotObjects) {
    server::Session session;

    auto first = make_test_object();
    auto second = make_test_object();
    second.objectID = "obj-2";

    session.addSnapshot(first);
    session.addSnapshot(second);

    ASSERT_EQ(session.getSnapshot().getmap().size(), 2);

    session.reset();

    EXPECT_TRUE(session.getSnapshot().getmap().empty());
}

TEST(SessionManagerTests, SessionManagerFindsFirstFreePort) {
    server::NetBus bus;
    server::SessionManager manager(bus);

    EXPECT_EQ(manager.find_free_port(), 30001);
}

TEST(SessionTests, NewSessionHasValidID) {
    server::Session session;

    EXPECT_TRUE(session.get_id().isValid());
}

TEST(SessionTests, SetJoinableUpdatesState) {
    server::Session session;

    session.setJoinable(true);
    EXPECT_TRUE(session.isGuestJoinable());

    session.setJoinable(false);
    EXPECT_FALSE(session.isGuestJoinable());
}

TEST(SessionTests, AddSnapshotStoresObject) {
    server::Session session;

    auto obj = make_test_object();
    session.addSnapshot(obj);

    auto snapshot = session.getSnapshot();

    auto* result = snapshot.find("obj-1");
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->name, "Object");
}

TEST(SessionTests, EditSnapshotChangesValues) {
    server::Session session;

    auto obj = make_test_object();
    session.addSnapshot(obj);

    auto snapshot = session.getSnapshot();

    auto obj2 = make_test_object();
    obj2.name = "Edited";

    session.editSnapshot(obj2);

    auto snapshot2 = session.getSnapshot();
    auto* result1 = snapshot.find("obj-1");
    auto* result2 = snapshot2.find("obj-1");

    ASSERT_NE(result1, nullptr);
    ASSERT_NE(result2, nullptr);
    EXPECT_NE(result1->name, result2->name);
}

TEST(SessionTests, DeleteSnapshotDeletesObject) {
    server::Session session;

    auto obj = make_test_object();
    session.addSnapshot(obj);

    auto snapshot = session.getSnapshot();

    session.deleteFromSnapshot(obj);

    auto snapshot2 = session.getSnapshot();

    auto* result = snapshot.find("obj-1");

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(snapshot2.find("obj-1"), nullptr);
}