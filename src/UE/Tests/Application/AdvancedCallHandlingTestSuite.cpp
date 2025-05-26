#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "States/DiallingState.hpp"
#include "States/ReceivingCallState.hpp"
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

class AdvancedCallHandlingTestSuite : public Test
{
protected:
    const common::PhoneNumber RECIPIENT_NUMBER{234};
    const common::PhoneNumber CALLER_NUMBER{123};
    const common::PhoneNumber THIRD_PARTY_NUMBER{121};
    const std::string TEST_SMS_TEXT = "This is a test SMS";
    
    NiceMock<common::ILoggerMock> loggerMock;
    NiceMock<IBtsPortMock> btsPortMock;
    NiceMock<IUserPortMock> userPortMock;
    NiceMock<ITimerPortMock> timerPortMock;
    NiceMock<ICallModeMock> callModeMock;
    NiceMock<ITextModeMock> textModeMock;
    NiceMock<IListViewModeMock> listViewModeMock;
    NiceMock<ISmsComposeModeMock> smsComposeModeMock;
    
    Context context{loggerMock, btsPortMock, userPortMock, timerPortMock};
    IUeGui::Callback acceptCallback;
    IUeGui::Callback rejectCallback;
    
    AdvancedCallHandlingTestSuite()
    {
        ON_CALL(userPortMock, setCallMode()).WillByDefault(ReturnRef(callModeMock));
        ON_CALL(userPortMock, showViewTextMode()).WillByDefault(ReturnRef(textModeMock));
        ON_CALL(userPortMock, getListViewMode()).WillByDefault(ReturnRef(listViewModeMock));
        ON_CALL(userPortMock, getSmsComposeMode()).WillByDefault(ReturnRef(smsComposeModeMock));
        
        EXPECT_CALL(userPortMock, setAcceptCallback(_))
            .WillOnce(SaveArg<0>(&acceptCallback));
        EXPECT_CALL(userPortMock, setRejectCallback(_))
            .WillOnce(SaveArg<0>(&rejectCallback));
    }
};

TEST_F(AdvancedCallHandlingTestSuite, shallHandleSmsWhileDialling)
{
    DiallingState objectUnderTest{context};
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    
    EXPECT_CALL(userPortMock, showNewSms(true));
    objectUnderTest.handleSms(THIRD_PARTY_NUMBER, TEST_SMS_TEXT);
}

TEST_F(AdvancedCallHandlingTestSuite, shallRejectCallRequestWhileDialling)
{
    DiallingState objectUnderTest{context};
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    

    EXPECT_CALL(btsPortMock, sendCallDropped(THIRD_PARTY_NUMBER));
    objectUnderTest.handleCallRequest(THIRD_PARTY_NUMBER);
}

TEST_F(AdvancedCallHandlingTestSuite, shallHandleSmsWhileReceivingCall)
{
    ReceivingCallState objectUnderTest{context, CALLER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);

    EXPECT_CALL(userPortMock, showNewSms(true));
    objectUnderTest.handleSms(THIRD_PARTY_NUMBER, TEST_SMS_TEXT);
}

TEST_F(AdvancedCallHandlingTestSuite, shallRejectCallRequestWhileReceivingCall)
{
    ReceivingCallState objectUnderTest{context, CALLER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    
    EXPECT_CALL(btsPortMock, sendCallDropped(THIRD_PARTY_NUMBER));
    objectUnderTest.handleCallRequest(THIRD_PARTY_NUMBER);
}

TEST_F(AdvancedCallHandlingTestSuite, shallHandleSmsWhileTalking)
{
    TalkingState objectUnderTest{context, CALLER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);

    EXPECT_CALL(userPortMock, showNewSms(true));
    objectUnderTest.handleSms(THIRD_PARTY_NUMBER, TEST_SMS_TEXT);
}

TEST_F(AdvancedCallHandlingTestSuite, shallRejectCallRequestWhileTalking)
{
    TalkingState objectUnderTest{context, CALLER_NUMBER};
    ::testing::Mock::VerifyAndClearExpectations(&btsPortMock);
    ::testing::Mock::VerifyAndClearExpectations(&userPortMock);
    
    EXPECT_CALL(btsPortMock, sendCallDropped(THIRD_PARTY_NUMBER));
    objectUnderTest.handleCallRequest(THIRD_PARTY_NUMBER);
}

}
