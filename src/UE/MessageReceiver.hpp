#pragma once

class Message;  // forward declaration

class MessageReceiver {
public:
    void receiveMessage(const Message& msg);
};
