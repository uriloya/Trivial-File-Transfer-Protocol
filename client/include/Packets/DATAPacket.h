#pragma once

#include "BasePacket.h"
#include <string>
#include <vector>
#include <iostream>
#include <string.h>

class DATAPacket : public BasePacket {
private:
    char* data;
    short packetSize;
    short blockNum ;
    DATAPacket(const DATAPacket &dATAPacket);

public:
    DATAPacket(short opCode, short size, short block, char bytes[]);

    DATAPacket(short size, short block, char bytes[]);

    short getPacketSize();

    short getBlockNum();
    virtual ~DATAPacket();

    char* getData();


};

