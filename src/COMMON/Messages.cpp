
#include "Messages.hpp"

Message::Message(const std::string& content)
    : content_(content) {}

std::string Message::getContent() const {
    return content_;
}
