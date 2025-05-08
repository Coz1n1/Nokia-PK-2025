#include "UserEquipment.hpp"

void UserEquipment::onDataReceived(const Message& msg) {
    messageReceiver.receiveMessage(msg);
}
