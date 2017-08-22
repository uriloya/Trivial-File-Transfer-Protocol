#include <iostream>
#include "../../include/Packets/ACKPacket.h"


ACKPacket::ACKPacket(const ACKPacket &ackPacket):blockNum(ackPacket.blockNum) {}


ACKPacket::ACKPacket():blockNum(0) {
    this->opCode = 4;
}

ACKPacket::ACKPacket(short blockNum):blockNum(blockNum) {
    this->opCode = 4;
}

short ACKPacket::getBlockNum() {
    return blockNum;
}

void ACKPacket::printACK() {
    std::cout << "ACK " << blockNum << std::endl;
}

ACKPacket::~ACKPacket() {}
