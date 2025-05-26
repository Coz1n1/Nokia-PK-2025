#include "TalkingState.hpp"
#include "ConnectedState.hpp"
#include "SmsDb.hpp"
#include "SharedSmsDb.hpp"

namespace ue
{

TalkingState::TalkingState(Context &context, common::PhoneNumber peer)
    : BaseState(context, "TalkingState"),
      peerPhoneNumber(peer)
{
    logger.logInfo("Talking with: ", peerPhoneNumber);
    auto& callMode = context.user.setCallMode();
    callMode.clearIncomingText();
    callMode.clearOutgoingText();
    
    context.user.setAcceptCallback([this]() {
        const std::string text = this->context.user.setCallMode().getOutgoingText();
        if (!text.empty()) {
            this->context.bts.sendCallTalk(peerPhoneNumber, text);
            this->context.user.setCallMode().clearOutgoingText();
        }
    });
    
    context.user.setRejectCallback([this]() {
        logger.logInfo("Call dropped");
        this->context.bts.sendCallDropped(peerPhoneNumber);
        this->context.setState<ConnectedState>();
    });
}

void TalkingState::handleDisconnected()
{
    context.setState<ConnectedState>();
}

void TalkingState::handleCallDropped(common::PhoneNumber from)
{
    if (from == peerPhoneNumber) {
        logger.logInfo("Peer ended the call: ", from);
        context.user.showViewTextMode().setText("Call ended by: " + to_string(from));
        context.setState<ConnectedState>();
    }
}

void TalkingState::handleCallTalk(common::PhoneNumber from, const std::string& text)
{
    if (from == peerPhoneNumber) {
        logger.logInfo("Received talk from: ", from, ", text: ", text);
        context.user.setCallMode().appendIncomingText(text);
    }
}

void TalkingState::handleHomeClicked()
{
    logger.logInfo("Call dropped due to home button");
    context.bts.sendCallDropped(peerPhoneNumber);
    context.setState<ConnectedState>();
}

void TalkingState::showConnectedView()
{
    context.setState<ConnectedState>();
}

void TalkingState::handleSms(common::PhoneNumber from, const std::string& text)
{
    logger.logInfo("Received SMS during active call from: ", from, ", text: ", text);
    

    auto& smsDb = SharedSmsDb::getInstance();
    smsDb.addSms(from, text);
    
    context.user.showNewSms(true);
}

void TalkingState::handleCallRequest(common::PhoneNumber from)
{
    logger.logInfo("Received call request during active call, rejecting request from: ", from);
    context.bts.sendCallDropped(from);
}

}