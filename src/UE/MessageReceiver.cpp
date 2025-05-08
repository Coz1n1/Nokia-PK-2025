#include "MessageReceiver.hpp"
#include "../COMMON/Messages.hpp"  // lub tylko "Messages.hpp", jeśli masz include_directories

#include <iostream>

void MessageReceiver::receiveMessage(const Message& msg) {
    std::cout << "Odebrano wiadomość: " << msg.getContent() << std::endl;
}
