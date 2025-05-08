#pragma once

#include "Messages/MessageHeader.hpp"
#include "Messages/BinaryMessage.hpp"

#pragma once
#include <string>

class Message {
public:
    explicit Message(const std::string& content);
    std::string getContent() const;

private:
    std::string content_;
};
