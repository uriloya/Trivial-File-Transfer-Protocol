//
// Created by ניר דוניץ on 15.1.2017.
//


#include <boost/thread/pthread/thread_data.hpp>
#include "../include/ServerListener.h"
#include "../include/Packets/BCASTPacket.h"
#include "../include/Packets/ACKPacket.h"
#include <fstream>
#include <boost/filesystem/operations.hpp>

using std::ifstream;
using std::ofstream;
using std::ios;
using std::streampos;
using std::cout;
using std::endl;
using namespace std;

#include <iostream>
#include <boost/thread/thread.hpp>

ServerListener::ServerListener(ConnectionHandler &handler) :
        _handler(handler),
        dataFromServer() {
    disconnectedReq = false;

}

void ServerListener::run() {
    while (!_handler.shouldTerminate() && !disconnectedReq) {
        BasePacket *packetFromServer;

        packetFromServer = _handler.processServerPakect();
        createResponse(packetFromServer);

    }

}

void ServerListener::createResponse(BasePacket *packetFromServer) {
    short currentAct = _handler.getCurrentAction();
    switch (packetFromServer->getOpCode()) {
        //DATA

        case 3: {
            char *data = (static_cast<DATAPacket *>(packetFromServer))->getData();
            int size = (static_cast<DATAPacket *>(packetFromServer))->getPacketSize();
            dataFromServer.insert(dataFromServer.end(), data, data + size);

            short dataSize = (static_cast<DATAPacket *>(packetFromServer))->getPacketSize();
            short blockNumber = (static_cast<DATAPacket *>(packetFromServer))->getBlockNum();

            //downloading
            if (currentAct == 1) {
                /////


                std::ofstream stream;
                string currFileName = _handler.getFileName();
                stream.open(_handler.getFileName(), ios::out | ios::app | ios::binary);
                if (stream.is_open()) {
                    if (dataSize > 0) {
                        stream.write(data, dataSize);
                    }
                    stream.close();
                }
                std::string blockString = to_string(blockNumber);
                _handler.encodeAndSend("ACK " + blockString);
                if (dataSize < 512) {
                    dataFromServer.clear();
                    std::cout << "RRQ " << currFileName << " complete" << std::endl;
                }
                delete (data);

            }
                //dirc
            else if (currentAct == 6) {
                if (dataSize != 512) {
                    std::vector<char>::iterator it = dataFromServer.begin();
                    for (; it != dataFromServer.end(); it++) {
                        if (it.operator*() == '\0') {
                            it.operator*() = '\n';
                        }
                    }


                    std::string printDir(dataFromServer.begin(), dataFromServer.end());
                    std::cout << printDir << std::endl;
                    dataFromServer.clear();
                }
            }
            break;
        }

            //ACK
        case 4: {
            switch (currentAct) {
                //WRQ
                case 2: {
                    (static_cast<ACKPacket *>(packetFromServer))->printACK();
                    short blockNum = (static_cast<ACKPacket *>(packetFromServer))->getBlockNum();
                    //open "filename" read from it to map of vectors of chars and send first packet
                    std::ifstream stream;
                    stream.open(_handler.getFileName(), ios::in | ios::binary | ios::ate);
                    if (stream.is_open()) {
                        streampos fileSize = stream.tellg();
                        unsigned startFrom = (unsigned) (512 * blockNum);

                        //
                        if (startFrom <= fileSize) {
                            stream.seekg(startFrom, ios::beg);
                            unsigned int leftToRead = ((((unsigned) fileSize - startFrom) <= 512) ?
                                                        ((unsigned) fileSize - startFrom) : 512);

                            char *dataBytes = new char[leftToRead];
                            stream.read(dataBytes, leftToRead);
                            stream.close();

                            //creqte
                            char blockNumArr[2];
                            shortToBytes(blockNum + 1, blockNumArr);
                            char opCodeArr[2];
                            shortToBytes((short) 3, opCodeArr);
                            char leftToReadArr[2];
                            shortToBytes(leftToRead, leftToReadArr);
                            char dataBytesPacket[leftToRead + 6];
                            _handler.mergeArrays(dataBytesPacket, opCodeArr, 2, 0);
                            _handler.mergeArrays(dataBytesPacket, leftToReadArr, 2, 2);
                            _handler.mergeArrays(dataBytesPacket, blockNumArr, 2, 4);
                            _handler.mergeArrays(dataBytesPacket, dataBytes, leftToRead, 6);
                            _handler.sendBytes(dataBytesPacket, leftToRead + 6);
                            delete[]dataBytes;
                        } else {
                            std::cout << "WRQ " << _handler.getFileName() << " complete" << std::endl;
                        }
                    } else {

                    }
                    break;
                }
//                LOGIN
                case 7: {
                    short blockNum = (static_cast<ACKPacket *>(packetFromServer))->getBlockNum();
                    if (blockNum == 0) {
                        (static_cast<ACKPacket *>(packetFromServer))->printACK();
                    }
                    break;
                }
                    //DELRQ
                case 8: {
                    short blockNum = (static_cast<ACKPacket *>(packetFromServer))->getBlockNum();
                    if (blockNum == 0) {
                        (static_cast<ACKPacket *>(packetFromServer))->printACK();
                    } else {
                        std::cout << "Wrong Ack Block Number inside server listener" << std::endl;
                    }
                    break;
                }
                    //DISC
                case 10: {
                    short blockNum = (static_cast<ACKPacket *>(packetFromServer))->getBlockNum();
                    if (blockNum == 0) {
                        (static_cast<ACKPacket *>(packetFromServer))->printACK();
                        _handler.terminate();
                    } else {
                        std::cout << "cant disconnect" << std::endl;
                    }
                    disconnectedReq = true;
                    break;
                }

            }
            break;

        }
            //ERROR
        case 5: {
            static_cast<ERRORPacket *> (packetFromServer)->printError();
            break;

        }
            //BCAST
        case 9: {
            static_cast<BCASTPacket *> (packetFromServer)->printMessage();
            break;
        }
        default:
            break;
    }

}

void ServerListener::shortToBytes(short num, char *bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

ServerListener::~ServerListener(){}
