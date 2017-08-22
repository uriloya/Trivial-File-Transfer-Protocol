//
// Created by ניר דוניץ on 15.1.2017.
//
#pragma once
#ifndef CLIENT_KeyBoardListener_H
#define CLIENT_KeyBoardListener_H

#include <boost/thread.hpp>
#include "ConnectionHandler.h"

class KeyBoardListener {
public:
    virtual ~KeyBoardListener();
    KeyBoardListener(ConnectionHandler &handler);

    void run();

private:
    ConnectionHandler &_handler;
    const short _bufferSize;
    bool disconnectedReq;



};

#include <iostream>
#include <boost/thread.hpp>


#endif //CLIENT_KeyBoardListener_H
