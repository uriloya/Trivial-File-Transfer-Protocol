#include "../../include/Packets/BasePacket.h"


short BasePacket::getOpCode() {
    return opCode;
}

BasePacket::BasePacket() {
}
BasePacket::~BasePacket() {}
BasePacket::BasePacket(const BasePacket &basePacket):opCode(basePacket.opCode) {}

