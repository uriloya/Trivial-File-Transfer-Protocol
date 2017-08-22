#pragma once

#include "BasePacket.h"
#include <string>
#include <vector>


class BCASTPacket : public BasePacket {
private:
    std::string Filename;
    bool fileAdded ;

public:
    BCASTPacket(char addOrDelete, const std::string &filename);

    BCASTPacket(const std::string &filename);
    BCASTPacket(const BCASTPacket &bCASTPacket);

    virtual ~BCASTPacket();
    void printMessage();

};
