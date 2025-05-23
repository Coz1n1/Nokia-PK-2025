#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ReceivingCallState.hpp"
#include "DiallingState.hpp"

namespace ue
{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    showMenuView();
}

void ConnectedState::handleDisconnected()
{
    context.setState<NotConnectedState>();
}

void ConnectedState::handleSms(common::PhoneNumber from, const std::string& text)
{
    logger.logInfo("Received SMS from: ", from, ", text: ", text);
    smsDb.addSms(from, text);
    refreshMessageIndicator();
}

void ConnectedState::handleCallRequest(common::PhoneNumber from)
{
    logger.logInfo("Received call request from: ", from);
    common::PhoneNumber caller = from;
    context.setState<ReceivingCallState>(caller);
}

void ConnectedState::handleHomeClicked()
{
    showMenuView();
}

void ConnectedState::handleSmsComposeClicked()
{
    showSmsComposerView();
}

void ConnectedState::handleSmsViewClicked()
{
    showSmsListView();
}

void ConnectedState::handleDialClicked()
{
    logger.logInfo("Dial clicked - transitioning to dialing state");
    context.setState<DiallingState>();
}

void ConnectedState::showMenuView()
{
    context.user.showConnected();
    
    auto& listView = context.user.getListViewMode();
    listView.clearSelectionList();
    listView.addSelectionListItem("Compose SMS", "Create a new SMS message");
    listView.addSelectionListItem("View SMS", "View received messages");
    listView.addSelectionListItem("Dial", "Make a call");
    
    context.user.setAcceptCallback([this]() {
        auto selectedItem = context.user.getListViewMode().getCurrentItemIndex();
        if (selectedItem.first) {
            switch (selectedItem.second) {
                case 0:  // Compose SMS
                    handleSmsComposeClicked();
                    break;
                case 1:  // View SMS
                    handleSmsViewClicked();
                    break;
                case 2:  // Dial
                    handleDialClicked();
                    break;
                default:
                    break;
            }
        }
    });
}

void ConnectedState::showSmsListView()
{
    context.user.showSmsListView();
    
    currentMessagesList = smsDb.getSmsMessages();
    auto& menu = context.user.getListViewMode();
    
    for (size_t i = 0; i < currentMessagesList.size(); ++i)
    {
        const auto& message = currentMessagesList[i];
        std::string label;
        if (message.isSent) {
            label = "To: " + to_string(message.to);
        } else {
            label = "From: " + to_string(message.from);
        }
        std::string tooltip = message.text;
        menu.addSelectionListItem(label, tooltip);
        
        if (!message.isRead)
        {
            smsDb.markAsRead(i);
        }
    }
    
    context.user.setAcceptCallback([this]() {
        auto& listView = context.user.getListViewMode();
        auto selectedItem = listView.getCurrentItemIndex();
        
        if (selectedItem.first && selectedItem.second < currentMessagesList.size()) {
            showSmsView(currentMessagesList[selectedItem.second]);
        }
    });
    
    refreshMessageIndicator();
}

void ConnectedState::showSmsComposerView()
{
    context.user.showSmsComposerView();
    
    auto& composeMode = context.user.getSmsComposeMode();
    composeMode.clearSmsText();
    
    context.user.setAcceptCallback([this]() {
        this->handleSmsSend();
    });
}

void ConnectedState::handleSmsSend()
{
    auto& composeMode = context.user.getSmsComposeMode();
    PhoneNumber recipient = composeMode.getPhoneNumber();
    std::string text = composeMode.getSmsText();
    
    logger.logInfo("Sending SMS to: ", recipient, ", text: ", text);
    
    context.bts.sendSms(recipient, text);
    
    smsDb.addSentSms(recipient, text);
    
    composeMode.clearSmsText();
    
    showMenuView();
}

void ConnectedState::showSmsView(const SmsMessage& message)
{
    auto& textMode = context.user.showViewTextMode();
    
    std::string header;
    if (message.isSent) {
        header = "Sent message to: " + to_string(message.to);
    } else {
        header = "Message from: " + to_string(message.from);
    }
    
    textMode.setText(header + "\n\n" + message.text);
    
    context.user.setAcceptCallback([this]() {
        showSmsListView();
    });
    
    context.user.setRejectCallback([this]() {
        showMenuView();
    });
}

void ConnectedState::refreshMessageIndicator()
{
    bool hasUnread = smsDb.hasUnreadSms();
    context.user.showNewSms(hasUnread);
}

}