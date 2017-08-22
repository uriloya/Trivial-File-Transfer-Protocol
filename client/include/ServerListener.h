//
// Created by ניר דוניץ on 15.1.2017.
//
#pragma once
#ifndef CLIENT_SERVERLISTENER_H
#define CLIENT_SERVERLISTENER_H

#include <boost/thread.hpp>

#include "ConnectionHandler.h"

class ServerListener {
private:

    ConnectionHandler &_handler;
    std::vector<char> dataFromServer;
    void shortToBytes(short num, char *bytesArr);
    bool disconnectedReq;
public:
    ServerListener(ConnectionHandler& handler);
    void run();

    void createResponse(BasePacket *packet);

    virtual ~ServerListener();

};

#endif //CLIENT_SERVERLISTENER_H
