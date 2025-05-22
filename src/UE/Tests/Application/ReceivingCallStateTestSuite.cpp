#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "States/ReceivingCallState.hpp"
#include "States/ConnectedState.hpp"
#include "States/TalkingState.hpp"
#include "Context.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Mocks/IUeGuiMock.hpp"

namespace ue
{
using namespace ::testing;

class ReceivingCallStateTestSuite : public Test
{
protected:
    const common::PhoneNumber CALLER_NUMBER{123};
    NiceMock<common::ILoggerMock> loggerMock;
    NiceMock<IBtsPortMock> btsPortMock;
    NiceMock<IUserPortMock> userPortMock;
    NiceMock<ITimerPortMock> timerPortMock;
    NiceMock<ITextModeMock> textModeMock;
    NiceMock<IListViewModeMock> listViewModeMock;
    NiceMock<ICallModeMock> callModeMock;
    
    Context context{loggerMock, btsPortMock, userPortMock, timerPortMock};
    IUeGui::Callback acceptCallback;
    IUeGui::Callback rejectCallback;
    
    ReceivingCallStateTestSuite()
    {
        ON_CALL(userPortMock, showViewTextMode()).WillByDefault(ReturnRef(textModeMock));
        ON_CALL(userPortMock, getListViewMode()).WillByDefault(ReturnRef(listViewModeMock));
        ON_CALL(userPortMock, setCallMode()).WillByDefault(ReturnRef(callModeMock));
        
        EXPECT_CALL(userPortMock, setAcceptCallback(_))
            .WillOnce(SaveArg<0>(&acceptCallback));
        EXPECT_CALL(userPortMock, setRejectCallback(_))
            .WillOnce(SaveArg<0>(&rejectCallback));
    }
};

TEST_F(ReceivingCallStateTestSuite, shallShowIncomingCallScreenAndStartTimer)
{
    EXPECT_CALL(userPortMock, showViewTextMode());
    EXPECT_CALL(textModeMock, setText(HasSubstr("Incoming call from: " + to_string(CALLER_NUMBER))));
    EXPECT_CALL(timerPortMock, startTimer(_));
    
    ReceivingCallState objectUnderTest{context, CALLER_NUMBER};
}

TEST_F(ReceivingCallStateTestSuite, shallAcceptCallAndTransitionToTalkingState)
{
    ReceivingCallState objectUnderTest{context, CALLER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(btsPortMock, sendCallAccepted(CALLER_NUMBER));
    EXPECT_CALL(timerPortMock, stopTimer());
    
    acceptCallback();
}

TEST_F(ReceivingCallStateTestSuite, shallRejectCallAndTransitionToConnectedState)
{
    ReceivingCallState objectUnderTest{context, CALLER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(btsPortMock, sendCallDropped(CALLER_NUMBER));
    EXPECT_CALL(userPortMock, showConnected());
    EXPECT_CALL(timerPortMock, stopTimer());
    
    rejectCallback();
}

TEST_F(ReceivingCallStateTestSuite, shallTimeoutAndRejectCall)
{
    ReceivingCallState objectUnderTest{context, CALLER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(btsPortMock, sendCallDropped(CALLER_NUMBER));
    EXPECT_CALL(userPortMock, showConnected());
    EXPECT_CALL(timerPortMock, stopTimer());
    
    objectUnderTest.handleTimeout();
}

TEST_F(ReceivingCallStateTestSuite, shallRejectCallOnHomeButton)
{
    ReceivingCallState objectUnderTest{context, CALLER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(btsPortMock, sendCallDropped(CALLER_NUMBER));
    EXPECT_CALL(userPortMock, showConnected());
    EXPECT_CALL(timerPortMock, stopTimer());
    
    objectUnderTest.handleHomeClicked();
    
}

}