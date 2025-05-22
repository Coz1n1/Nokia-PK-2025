#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "States/DiallingState.hpp"
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

class DiallingStateTestSuite : public Test
{
protected:
    const common::PhoneNumber RECIPIENT_NUMBER{234};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsPortMock> btsPortMock;
    StrictMock<IUserPortMock> userPortMock;
    StrictMock<ITimerPortMock> timerPortMock;
    StrictMock<ICallModeMock> callModeMock;
    StrictMock<ITextModeMock> textModeMock;
    
    Context context{loggerMock, btsPortMock, userPortMock, timerPortMock};
    IUeGui::Callback acceptCallback;
    IUeGui::Callback rejectCallback;
    
    DiallingStateTestSuite()
    {
        ON_CALL(userPortMock, setCallMode()).WillByDefault(ReturnRef(callModeMock));
        ON_CALL(userPortMock, showViewTextMode()).WillByDefault(ReturnRef(textModeMock));
        
        EXPECT_CALL(userPortMock, setAcceptCallback(_))
            .WillOnce(SaveArg<0>(&acceptCallback));
        EXPECT_CALL(userPortMock, setRejectCallback(_))
            .WillOnce(SaveArg<0>(&rejectCallback));
    }
};

TEST_F(DiallingStateTestSuite, shallShowDialScreenAndStartTimer)
{
    EXPECT_CALL(userPortMock, setCallMode());
    EXPECT_CALL(userPortMock, showViewTextMode());
    EXPECT_CALL(textModeMock, setText(_));
    EXPECT_CALL(callModeMock, clearIncomingText());
    EXPECT_CALL(callModeMock, clearOutgoingText());
    EXPECT_CALL(timerPortMock, startTimer(_));
    
    DiallingState objectUnderTest{context};
}

TEST_F(DiallingStateTestSuite, shallSendCallRequestWhenUserAccepts)
{
    DiallingState objectUnderTest{context};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return(std::to_string(RECIPIENT_NUMBER.value)));
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(HasSubstr("Calling: " + to_string(RECIPIENT_NUMBER))));
    EXPECT_CALL(btsPortMock, sendCallRequest(RECIPIENT_NUMBER));
    
    acceptCallback();
}

TEST_F(DiallingStateTestSuite, shallHandleInvalidPhoneNumber)
{
    DiallingState objectUnderTest{context};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return(""));
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(HasSubstr("Invalid phone number")));
    
    EXPECT_CALL(btsPortMock, sendCallRequest(_)).Times(0);
    
    acceptCallback();
}

TEST_F(DiallingStateTestSuite, shallTransitionToTalkingStateWhenCallAccepted)
{
    DiallingState objectUnderTest{context};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return(std::to_string(RECIPIENT_NUMBER.value)));
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(_));
    acceptCallback();
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(timerPortMock, stopTimer());
    
    objectUnderTest.handleCallAccepted(RECIPIENT_NUMBER);
}

TEST_F(DiallingStateTestSuite, shallHandleCallDroppedByRecipient)
{
    DiallingState objectUnderTest{context};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return(std::to_string(RECIPIENT_NUMBER.value)));
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(_));
    acceptCallback();
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(HasSubstr("Call rejected by: " + to_string(RECIPIENT_NUMBER))));
    EXPECT_CALL(timerPortMock, stopTimer());
    
    objectUnderTest.handleCallDropped(RECIPIENT_NUMBER);
}

TEST_F(DiallingStateTestSuite, shallHandleUnknownRecipient)
{
    DiallingState objectUnderTest{context};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return(std::to_string(RECIPIENT_NUMBER.value)));
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(_));
    acceptCallback();
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(HasSubstr("User not available")));
    EXPECT_CALL(timerPortMock, stopTimer());
    
    objectUnderTest.handleUnknownRecipient();
}

TEST_F(DiallingStateTestSuite, shallHandleTimeout)
{
    DiallingState objectUnderTest{context};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return(std::to_string(RECIPIENT_NUMBER.value)));
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(_));
    acceptCallback();
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(HasSubstr("Call timed out")));
    EXPECT_CALL(btsPortMock, sendCallDropped(RECIPIENT_NUMBER));
    EXPECT_CALL(timerPortMock, stopTimer());
    
    objectUnderTest.handleTimeout();
}

TEST_F(DiallingStateTestSuite, shallCancelCallWhenUserRejects)
{
    DiallingState objectUnderTest{context};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return(std::to_string(RECIPIENT_NUMBER.value)));
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(_));
    acceptCallback();
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(btsPortMock, sendCallDropped(RECIPIENT_NUMBER));
    EXPECT_CALL(timerPortMock, stopTimer());
    
    rejectCallback();
}

TEST_F(DiallingStateTestSuite, shallCancelCallOnHomeButton)
{
    DiallingState objectUnderTest{context};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return(std::to_string(RECIPIENT_NUMBER.value)));
    EXPECT_CALL(userPortMock, showViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText(_));
    EXPECT_CALL(btsPortMock, sendCallRequest(_));
    acceptCallback();
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(btsPortMock, sendCallDropped(RECIPIENT_NUMBER));
    EXPECT_CALL(timerPortMock, stopTimer());
    
    objectUnderTest.handleHomeClicked();
}

}