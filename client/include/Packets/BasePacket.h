#pragma once

#include <vector>

class BasePacket {
protected:
    short opCode;
public:
     short getOpCode();

    BasePacket();

    BasePacket(const BasePacket&);
    virtual ~BasePacket();
};