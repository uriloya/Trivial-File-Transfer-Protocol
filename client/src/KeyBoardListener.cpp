

#include "../include/KeyBoardListener.h"


KeyBoardListener::KeyBoardListener(ConnectionHandler &handler) :
        _handler(handler),
        _bufferSize(1024),disconnectedReq(false) {
}

void KeyBoardListener::run() {

    while (!_handler.shouldTerminate() && !disconnectedReq) {

        char buf[_bufferSize];
        std::cin.getline(buf, _bufferSize);

        std::string line(buf);
        if (!_handler.encodeAndSend(line)) {
            continue;
        }
        if(_handler.isLoggedIn()){
            disconnectedReq = line == "DISC";


        }

    }
}

KeyBoardListener::~KeyBoardListener() {

}
