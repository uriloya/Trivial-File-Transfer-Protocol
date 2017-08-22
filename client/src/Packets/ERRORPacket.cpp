#include <iostream>
#include "../../include/Packets/ERRORPacket.h"

ERRORPacket::ERRORPacket(short errorType):ErrorCode(errorType),ErrMsg("") {
    this->opCode=5;
    defineErrMsg();
}
ERRORPacket::~ERRORPacket(){}
ERRORPacket::ERRORPacket(const ERRORPacket &eRRORPacket):ErrMsg(eRRORPacket.ErrMsg) {}

ERRORPacket::ERRORPacket(short errorType, const std::string &errMsg):ErrorCode(errorType),ErrMsg(errMsg) {
    this->opCode=5;
}

void ERRORPacket::defineErrMsg() {
    switch (ErrorCode) {
        case 0:
            ErrMsg = "Not defined, see error message (if any).";
            break;
        case 1:
            ErrMsg = "File not found – RRQ \\ DELRQ of non-existing file.";
            break;
        case 2:
            ErrMsg = "Access violation – File cannot be written, read or deleted.";
            break;
        case 3:
            ErrMsg = "Disk full or allocation exceeded – No room in disk.";
            break;
        case 4:
            ErrMsg = "Illegal TFTP operation – Unknown Opcode.";
            break;
        case 5:
            ErrMsg = "File already exists – File name exists on WRQ.";
            break;
        case 6:
            ErrMsg = "User not logged in – Any opcode received before Login completes.";
            break;
        case 7:
            ErrMsg = "User already logged in – Login username already connected.";
            break;
        default:
            ErrMsg = "Wrong error code insert";
            break;

    }
}


void ERRORPacket::printError() {
    std::cout << "ERROR " << ErrorCode <<std::endl;
}