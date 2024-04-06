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
    MockTimerClient mockClient;
    Timer timer;

    TimedDoorTest() : door(1), timer() {}

    void SetUp() override {
        timer.tregister(door.getTimeOut(), &mockClient);
    }

    void TearDown() override {
        testing::Mock::VerifyAndClearExpectations(&mockClient);
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

TEST_F(TimedDoorTest, double_unlock) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, double_lock) {
    EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(TimedDoorTest, closed_bvefore_timeout) {
    EXPECT_NO_THROW(door.throwState());
}

TEST_F(TimedDoorTest, opened_exception_throw) {
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, lock_door_then_unlocked_after_timeout_no_exception) {
    std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut() + 1));
    EXPECT_NO_THROW(door.throwState());
}

TEST_F(TimedDoorTest, lock_before_timeout_then_opened_after_timeout_ex_throw) {
    std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut() + 1));
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, unlock_door_then_timeout_exception_thrown) {
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut()));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}
