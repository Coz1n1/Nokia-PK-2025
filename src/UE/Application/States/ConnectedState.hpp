#pragma once

#include "BaseState.hpp"
#include "SmsDb.hpp"
#include "SharedSmsDb.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context& context);

    // IBtsEventsHandler interface
    void handleDisconnected() override;
    void handleSms(common::PhoneNumber from, const std::string& text) override;
    void handleCallRequest(common::PhoneNumber from) override;
    
    // IUserEventsHandler interface
    void handleHomeClicked() override;
    void handleSmsComposeClicked() override;
    void handleSmsViewClicked() override;
    void handleDialClicked();
    
private:
    void showMenuView();
    void showSmsListView();
    void showSmsComposerView();
    void showSmsView(const SmsMessage& message);
    void handleSmsSend();
    void refreshMessageIndicator();
    
    std::vector<SmsMessage> currentMessagesList;
};

}