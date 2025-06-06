#pragma once

#include "UeGui/IListViewMode.hpp"
#include "UeGui/ISmsComposeMode.hpp"
#include "UeGui/ITextMode.hpp"
#include "UeGui/ICallMode.hpp"

namespace ue
{

class IUserEventsHandler
{
public:
    virtual ~IUserEventsHandler() = default;

    virtual void handleHomeClicked() = 0;
    virtual void handleSmsComposeClicked() = 0;
    virtual void handleSmsViewClicked() = 0;
};

class IUserPort
{
public:
    virtual ~IUserPort() = default;

    virtual void showNotConnected() = 0;
    virtual void showConnecting() = 0;
    virtual void showConnected() = 0;
    virtual void showNewSms(bool present) = 0;
    virtual void showSmsListView() = 0;
    virtual void showSmsComposerView() = 0;
    virtual IUeGui::IListViewMode& getListViewMode() = 0;
    virtual IUeGui::ISmsComposeMode& getSmsComposeMode() = 0;
    virtual IUeGui::ITextMode& showViewTextMode() = 0;
    virtual IUeGui::ICallMode& setCallMode() = 0;
    virtual void setAcceptCallback(IUeGui::Callback) = 0;
    virtual void setRejectCallback(IUeGui::Callback) = 0;
    virtual void setHomeCallback(IUeGui::Callback) = 0;
};

}
