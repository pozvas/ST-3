// Copyright 2024 Pozdnyakov Vasya

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]

#include "TimedDoor.h"


class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor door;
    MockTimerClient *mockClient;
    Timer timer;

    TimedDoorTest() : door(500), timer() {}

    void SetUp() override {
        mockClient = new MockTimerClient();
    }

    void TearDown() override {
        delete mockClient;
    }
};

TEST_F(TimedDoorTest, unlock) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, lock) {
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, init_lock) {
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, opened_exception_throw) {
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, closed_exception_throw) {
    door.lock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, double_unlock) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, double_lock) {
    EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(TimedDoorTest, timeout_throws_on_unlocked) {
    DoorTimerAdapter adapter(door);
    door.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, timeout_no_throws_on_locked) {
    DoorTimerAdapter adapter(door);
    EXPECT_NO_THROW(adapter.Timeout());
}

TEST_F(TimedDoorTest, timer_calls_timeout) {
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
    timer.tregister(100, mockClient);
}
