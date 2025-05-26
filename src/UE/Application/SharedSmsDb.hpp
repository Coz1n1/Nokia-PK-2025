#pragma once
#include "SmsDb.hpp"

namespace ue
{

class SharedSmsDb 
{
public:
    static SmsDb& getInstance() 
    {
        static SmsDb instance;
        return instance;
    }

    SharedSmsDb(const SharedSmsDb&) = delete;
    SharedSmsDb& operator=(const SharedSmsDb&) = delete;

private:
    SharedSmsDb() = default;
    ~SharedSmsDb() = default;
};

}  