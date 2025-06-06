#include "Application.hpp"
#include "States/NotConnectedState.hpp"

namespace ue
{

Application::Application(common::PhoneNumber phoneNumber,
                         common::ILogger &iLogger,
                         IBtsPort &bts,
                         IUserPort &user,
                         ITimerPort &timer)
    : context{iLogger, bts, user, timer},
      logger(iLogger, "[APP] ")
{
    logger.logInfo("Started");
    context.setState<NotConnectedState>();
}

Application::~Application()
{
    logger.logInfo("Stopped");
}

void Application::handleTimeout()
{
    context.state->handleTimeout();
}

void Application::handleSib(common::BtsId btsId)
{
    context.state->handleSib(btsId);
}

void Application::handleAttachAccept()
{
    context.state->handleAttachAccept();
}

void Application::handleAttachReject()
{
    context.state->handleAttachReject();
}

void Application::handleDisconnected()
{
    context.state->handleDisconnected();
}

void Application::handleSms(common::PhoneNumber from, const std::string& text)
{
    context.state->handleSms(from, text);
}

void Application::handleCallRequest(common::PhoneNumber from)
{
    context.state->handleCallRequest(from);
}

void Application::handleCallAccepted(common::PhoneNumber from)
{
    context.state->handleCallAccepted(from);
}

void Application::handleCallDropped(common::PhoneNumber from)
{
    context.state->handleCallDropped(from);
}

void Application::handleCallTalk(common::PhoneNumber from, const std::string& text)
{
    context.state->handleCallTalk(from, text);
}

void Application::handleUnknownRecipient()
{
    context.state->handleUnknownRecipient();
}

void Application::handleHomeClicked()
{
    context.state->handleHomeClicked();
}

void Application::handleSmsComposeClicked()
{
    context.state->handleSmsComposeClicked();
}

void Application::handleSmsViewClicked()
{
    context.state->handleSmsViewClicked();
}

}


