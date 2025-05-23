#include "BaseState.hpp"

namespace ue
{

BaseState::BaseState(Context &context, const std::string &name)
    : context(context),
      logger(context.logger, "[" + name + "]")
{
    logger.logDebug("entry");
}

BaseState::~BaseState()
{
    logger.logDebug("exit");
}

void BaseState::handleTimeout()
{
    logger.logError("Uexpected: handleTimeout");
}

void BaseState::handleSib(common::BtsId btsId)
{
    logger.logError("Uexpected: handleSib: ", btsId);
}

void BaseState::handleAttachAccept()
{
    logger.logError("Uexpected: handleAttachAccept");
}

void BaseState::handleAttachReject()
{
    logger.logError("Uexpected: handleAttachReject");
}

void BaseState::handleDisconnect()
{
    logger.logError("Uexpected: handleDisconnect");
}

void BaseState::handleSms(common::PhoneNumber from, const std::string& text)
{
    logger.logError("Unexpected: handleSms from ", from);
}

void BaseState::handleHomeClicked()
{
    logger.logError("Unexpected: handleHomeClicked");
}

void BaseState::handleSmsComposeClicked()
{
    logger.logError("Unexpected: handleSmsComposeClicked");
}

void BaseState::handleSmsViewClicked()
{
    logger.logError("Unexpected: handleSmsViewClicked");
}

}
