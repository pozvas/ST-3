#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

using namespace std::chrono_literals;

void Timer::sleep(int seconds) {
    std::this_thread::sleep_for(seconds * 1s);
}

void Timer::tregister(int seconds, TimerClient* client) {
    sleep(seconds);
    client->Timeout();
}

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("Door has been opened for too long!");
    }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const{
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Door is opened!");
    }
    else {
        throw std::runtime_error("Door is closed!");
    }
}