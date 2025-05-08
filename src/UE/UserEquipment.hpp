#pragma once

#include "MessageReceiver.hpp"

class UserEquipment {
private:
    MessageReceiver messageReceiver;
public:
    void onDataReceived(const Message& msg);
};
