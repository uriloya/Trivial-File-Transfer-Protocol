#pragma once

#include "BasePacket.h"
#include <string>

class ERRORPacket : public BasePacket {
private:
    std::string ErrMsg;
    short ErrorCode;
    ERRORPacket(const ERRORPacket &eRRORPacket);

    void defineErrMsg();

public:
    ERRORPacket(short errorType);

    ERRORPacket(short errorType, const std::string &errMsg);

    virtual ~ERRORPacket();
    void printError();

};