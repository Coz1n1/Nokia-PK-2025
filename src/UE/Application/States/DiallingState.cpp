#include "DiallingState.hpp"
#include "ConnectedState.hpp"
#include "TalkingState.hpp"
#include "NotConnectedState.hpp"
#include "SmsDb.hpp"
#include "SharedSmsDb.hpp"
#include <sstream>
#include <cstdlib>

namespace ue
{

const std::chrono::milliseconds DiallingState::CALL_TIMEOUT{60000}; // 60 seconds timeout

DiallingState::DiallingState(Context &context)
    : BaseState(context, "DiallingState")
{
    logger.logInfo("Dialling");
    showDialView();
    
    context.timer.startTimer(CALL_TIMEOUT);
}

DiallingState::~DiallingState()
{
    context.timer.stopTimer();
}

void DiallingState::showDialView()
{
    auto& textMode = context.user.showViewTextMode();
    textMode.setText("Enter phone number to call\nThen press green button to dial");

    auto& callMode = context.user.setCallMode();
    
    callMode.clearIncomingText();
    callMode.clearOutgoingText();
    
    context.user.setAcceptCallback([this]() {
        logger.logInfo("Call request initiated by user");
        sendCallRequest();
    });
    
    context.user.setRejectCallback([this]() {
        logger.logInfo("Dialing canceled by user");
        cancelCall();
    });
}

void DiallingState::sendCallRequest()
{
    auto& dialMode = context.user.setCallMode();
    std::string phoneNumberText = dialMode.getOutgoingText();
    logger.logDebug("Phone number entered: ", phoneNumberText);
    
    unsigned long numberValue = 0;
    try {
        if (!phoneNumberText.empty()) {
            numberValue = std::stoul(phoneNumberText);
        }
    } catch (const std::exception& ex) {
        logger.logError("Invalid phone number format: ", ex.what());
    }
    
    recipientPhoneNumber = common::PhoneNumber{numberValue};
    
    if (recipientPhoneNumber.value == 0)
    {
        logger.logError("Invalid recipient phone number");
        context.user.showViewTextMode().setText("Invalid phone number");
        context.setState<ConnectedState>();
        return;
    }
    
    logger.logInfo("Sending call request to: ", recipientPhoneNumber);
    context.user.showViewTextMode().setText("Calling: " + to_string(recipientPhoneNumber));
    
    context.bts.sendCallRequest(recipientPhoneNumber);
}

void DiallingState::handleCallAccepted(common::PhoneNumber from)
{
    if (from == recipientPhoneNumber)
    {
        logger.logInfo("Call accepted by: ", from);
        context.setState<TalkingState>(from);
    }
}

void DiallingState::handleCallDropped(common::PhoneNumber from)
{
    if (from == recipientPhoneNumber)
    {
        logger.logInfo("Call rejected by: ", from);
        context.user.showViewTextMode().setText("Call rejected by: " + to_string(from));
        context.setState<ConnectedState>();
    }
}

void DiallingState::handleUnknownRecipient()
{
    logger.logInfo("Unknown recipient: ", recipientPhoneNumber);
    context.user.showViewTextMode().setText("User not available: " + to_string(recipientPhoneNumber));
    context.setState<ConnectedState>();
}

void DiallingState::handleTimeout()
{
    logger.logInfo("Call request timed out");
    context.user.showViewTextMode().setText("Call timed out");
    
    if (recipientPhoneNumber.value != 0)
    {
        context.bts.sendCallDropped(recipientPhoneNumber);
    }
    
    context.setState<ConnectedState>();
}

void DiallingState::handleDisconnected()
{
    context.setState<NotConnectedState>();
}

void DiallingState::handleHomeClicked()
{
    cancelCall();
}

void DiallingState::cancelCall()
{
    if (recipientPhoneNumber.value != 0)
    {
        context.bts.sendCallDropped(recipientPhoneNumber);
    }
    
    context.setState<ConnectedState>();
}

void DiallingState::handleSms(common::PhoneNumber from, const std::string& text)
{
    logger.logInfo("Received SMS during dialling from: ", from, ", text: ", text);

    auto& smsDb = SharedSmsDb::getInstance();
    smsDb.addSms(from, text);
    
    context.user.showNewSms(true);
}

void DiallingState::handleCallRequest(common::PhoneNumber from)
{
    logger.logInfo("Received call request while dialling, ignoring request from: ", from);
    context.bts.sendCallDropped(from);
}

}