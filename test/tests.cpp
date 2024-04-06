// Copyright 2024 Pozdnyakov Vasya

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"


class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public testing::Test {
 public:
    TimedDoor* door;
    MockDoor* mockDoor;
    MockTimerClient* mockClient;
    void SetUp() override {
        mockDoor = new MockDoor();
        door = new TimedDoor(5);
        mockClient = new MockTimerClient();
    }

    void TearDown() override {
        delete mockDoor;
        delete door;
        delete mockClient;
    }
};

TEST_F(TimedDoorTest, DoorStartsClosed) {
    ASSERT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorOpensAndCloses) {
    door->unlock();
    ASSERT_TRUE(door->isDoorOpened());
    door->lock();
    ASSERT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutIsCalledWhenDoorIsOpenForTooLong) {
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
    door->unlock();
    testing::Mock::VerifyAndClearExpectations(&mockClient);
    testing::Mock::AllowLeak(&mockClient);
    testing::Mock::AllowLeak(&door);
}

TEST_F(TimedDoorTest, TimeoutThrowsExceptionWhenDoorIsStillOpen) {
    door->unlock();
    EXPECT_THROW(door->throwState(), const char*);
}
