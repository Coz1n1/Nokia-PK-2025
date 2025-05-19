#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "States/TalkingState.hpp"
#include "States/ConnectedState.hpp"
#include "Context.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Mocks/IUeGuiMock.hpp"

namespace ue
{
using namespace ::testing;

class TalkingStateTestSuite : public Test
{
protected:
    const common::PhoneNumber PEER_NUMBER{123};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsPortMock> btsPortMock;
    StrictMock<IUserPortMock> userPortMock;
    NiceMock<ITimerPortMock> timerPortMock;
    StrictMock<ICallModeMock> callModeMock;
    StrictMock<ITextModeMock> textModeMock;
    
    Context context{loggerMock, btsPortMock, userPortMock, timerPortMock};
    IUeGui::Callback acceptCallback;
    IUeGui::Callback rejectCallback;
    
    TalkingStateTestSuite()
    {
        ON_CALL(userPortMock, setCallMode()).WillByDefault(ReturnRef(callModeMock));
        ON_CALL(userPortMock, showViewTextMode()).WillByDefault(ReturnRef(textModeMock));
        
        EXPECT_CALL(userPortMock, setCallMode());
        EXPECT_CALL(callModeMock, clearIncomingText());
        EXPECT_CALL(callModeMock, clearOutgoingText());
        
        EXPECT_CALL(userPortMock, setAcceptCallback(_))
            .WillOnce(SaveArg<0>(&acceptCallback));
        EXPECT_CALL(userPortMock, setRejectCallback(_))
            .WillOnce(SaveArg<0>(&rejectCallback));
    }
};

TEST_F(TalkingStateTestSuite, shallSetupCallInterface)
{
    TalkingState objectUnderTest{context, PEER_NUMBER};
}

TEST_F(TalkingStateTestSuite, shallSendCallTalkOnAcceptWithText)
{
    const std::string TALK_TEXT = "Hello, this is a test call!";
    TalkingState objectUnderTest{context, PEER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&callModeMock);
    
    EXPECT_CALL(userPortMock, setCallMode());
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return(TALK_TEXT));
    EXPECT_CALL(btsPortMock, sendCallTalk(PEER_NUMBER, TALK_TEXT));
    EXPECT_CALL(userPortMock, setCallMode());
    EXPECT_CALL(callModeMock, clearOutgoingText());
    
    acceptCallback();
}

TEST_F(TalkingStateTestSuite, shallNotSendCallTalkOnAcceptWithEmptyText)
{
    const std::string EMPTY_TEXT = "";
    TalkingState objectUnderTest{context, PEER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&callModeMock);
    
    EXPECT_CALL(userPortMock, setCallMode());
    EXPECT_CALL(callModeMock, getOutgoingText()).WillOnce(Return(EMPTY_TEXT));
    EXPECT_CALL(btsPortMock, sendCallTalk(_, _)).Times(0);
    EXPECT_CALL(callModeMock, clearOutgoingText()).Times(0);
    
    acceptCallback();
}

TEST_F(TalkingStateTestSuite, shallEndCallOnReject)
{
    TalkingState objectUnderTest{context, PEER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(btsPortMock, sendCallDropped(PEER_NUMBER));
    
    rejectCallback();
}

TEST_F(TalkingStateTestSuite, shallEndCallOnHomeButton)
{
    TalkingState objectUnderTest{context, PEER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    
    EXPECT_CALL(btsPortMock, sendCallDropped(PEER_NUMBER));
    
    objectUnderTest.handleHomeClicked();
}

TEST_F(TalkingStateTestSuite, shallHandleReceivedCallTalkMessages)
{
    const std::string RECEIVED_TEXT = "Can you hear me?";
    TalkingState objectUnderTest{context, PEER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&callModeMock);
    
    EXPECT_CALL(userPortMock, setCallMode());
    EXPECT_CALL(callModeMock, appendIncomingText(RECEIVED_TEXT));
    
    objectUnderTest.handleCallTalk(PEER_NUMBER, RECEIVED_TEXT);
}

TEST_F(TalkingStateTestSuite, shallIgnoreCallTalkFromWrongNumber)
{
    const common::PhoneNumber WRONG_NUMBER{123};
    const std::string RECEIVED_TEXT = "Can you hear me?";
    TalkingState objectUnderTest{context, PEER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&callModeMock);
    
    EXPECT_CALL(callModeMock, appendIncomingText(_)).Times(0);
    
    objectUnderTest.handleCallTalk(WRONG_NUMBER, RECEIVED_TEXT);
}

TEST_F(TalkingStateTestSuite, shallHandleCallDroppedFromPeer)
{
    TalkingState objectUnderTest{context, PEER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&textModeMock);
    
    EXPECT_CALL(userPortMock, showViewTextMode());
    EXPECT_CALL(textModeMock, setText(HasSubstr("Call ended by: " + to_string(PEER_NUMBER))));

    objectUnderTest.handleCallDropped(PEER_NUMBER);
}

TEST_F(TalkingStateTestSuite, shallIgnoreCallDroppedFromWrongNumber)
{
    const common::PhoneNumber WRONG_NUMBER{123};
    TalkingState objectUnderTest{context, PEER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    
    EXPECT_CALL(userPortMock, showViewTextMode()).Times(0);
    
    objectUnderTest.handleCallDropped(WRONG_NUMBER);
}

}