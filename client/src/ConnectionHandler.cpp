

#include <iostream>
#include "../include/ConnectionHandler.h"
#include "../include/Packets/ACKPacket.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;


ConnectionHandler::ConnectionHandler(string host, short port) :
        host_(host),
        port_(port),
        io_service_(),
        socket_(io_service_),
        _currentAction(0),
        _connected(true),loggedIn(false) {
    encoderDecoder = new BidiEncoderDecoder();

}

ConnectionHandler::ConnectionHandler(ConnectionHandler &connectionHandler) :
        host_(connectionHandler.getHost()),
        port_(connectionHandler.getPort()),
        io_service_(),
        socket_(io_service_),
        _currentAction(0),
        _connected(true),
        encoderDecoder() ,loggedIn(false){

}

ConnectionHandler& ConnectionHandler::operator = (const ConnectionHandler &connectionHandler){
        return *this;
}

std::string ConnectionHandler::getHost() {
    return host_;

}

short ConnectionHandler::getPort() {
    return port_;
}


bool ConnectionHandler::shouldTerminate() {
    return !_connected;
}


short ConnectionHandler::getCurrentAction() {
    return _currentAction;
}


void ConnectionHandler::terminate() {
    _connected = false;
    close();
}

ConnectionHandler::~ConnectionHandler() {
}

bool ConnectionHandler::connect() {
    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error;
        socket_.connect(endpoint, error);
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception &e) {
        std::cerr << "Connection failed! (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp) {
            tmp += socket_.read_some(boost::asio::buffer(bytes + tmp, bytesToRead - tmp), error);
        }
        if (error)
            throw boost::system::system_error(error);
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp) {

            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
        if (error)
            throw boost::system::system_error(error);
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getLine(std::string &line) {
    return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string &line) {
    return sendFrameAscii(line, '\n');
}

bool ConnectionHandler::getFrameAscii(std::string &frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
        do {
            getBytes(&ch, 1);
            frame.append(1, ch);
        } while (delimiter != ch);
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendFrameAscii(const std::string &frame, char delimiter) {
    bool result = sendBytes(frame.c_str(), frame.length());
    if (!result) return false;
    return sendBytes(&delimiter, 1);
}

// Close down the connection properly.
void ConnectionHandler::close() {
    try {
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}
//ConnectionHandler::ConnectionHandler(const ConnectionHandler &handler) {
//
//}

bool ConnectionHandler::encodeAndSend(std::string line) {
    std::vector<char> packetBytes = encoderDecoder->encodeInputTobytes(line);

    //input qrong return false
    if(packetBytes.size()==0){
        return false;
    }

    std::vector<char>::iterator it;
    it = packetBytes.begin();


    char opCode[2];
    char toPush = it.operator*();
    opCode[0] = toPush;
    it++;
    char nextToPush = it.operator*();
    opCode[1] = nextToPush;
    updateCurrentAction(opCode);
    return sendBytes(packetBytes.data(), packetBytes.size());

}


//reading bytes according to op code
BasePacket *ConnectionHandler::processServerPakect() {
    char opCodeArr[2];
    getBytes(opCodeArr, 1);

    getBytes(opCodeArr + 1, 1);


    short opCode = encoderDecoder->bytesToShort(opCodeArr[0], opCodeArr[1]);
    std::vector<char> bytesToDecode;
    encoderDecoder->arrayToVector(&bytesToDecode, opCodeArr, 2);
    BasePacket *packetFromServer;


    switch (opCode) {
        //DATA
        case 3: {
            char sizeAndBlock[4];
            getBytes(sizeAndBlock, 4);
            encoderDecoder->arrayToVector(&bytesToDecode, sizeAndBlock, 4);
            short packetSize = encoderDecoder->bytesToShort(sizeAndBlock[0], sizeAndBlock[1]);
            char *data = new char[packetSize];
            getBytes(data, packetSize);
            encoderDecoder->arrayToVector(&bytesToDecode, data, packetSize);
            char packetToDecode[packetSize + 6];
            encoderDecoder->vectorToArray(bytesToDecode, packetToDecode);
            packetFromServer = encoderDecoder->decodeBytes(packetToDecode, packetSize + 6);
            break;
        }
            //ACK
        case 4: {
            char blockNumberACK[2];
            getBytes(&blockNumberACK[0], 1);
            getBytes(&blockNumberACK[1], 1);
            char packetToDecode[4];
            //void ConnectionHandler::mergeArrays(char *insertTo, char *insertFrom, int from) {
            mergeArrays(packetToDecode, opCodeArr, 2, 0);
            mergeArrays(packetToDecode, blockNumberACK, 2, 2);
//            short blockNumber = encoderDecoder->bytesToShort(blockNumberACK[0], blockNumberACK[1]);
            packetFromServer = encoderDecoder->decodeBytes(packetToDecode, 4);
            if(_currentAction==7){
                loggedIn=true;
            }
            break;
        }
            //ERROR
        case 5: {
            char errorCodeArr[2];
            getBytes(&errorCodeArr[0], 1);
            getBytes(&errorCodeArr[1], 1);
            encoderDecoder->arrayToVector(&bytesToDecode, errorCodeArr, 2);
//            short errorCode = encoderDecoder->bytesToShort(errorCodeArr[0], errorCodeArr[1]);
            std::vector<char> errorMsg = getBytesUntilDelimeter();
            bytesToDecode.insert(std::end(bytesToDecode), std::begin(errorMsg), std::end(errorMsg));
            int size = errorMsg.size() + 4;
            char *packetToEncode = new char[size];
            encoderDecoder->vectorToArray(bytesToDecode, packetToEncode);
            packetFromServer = encoderDecoder->decodeBytes(packetToEncode, size);
            break;
        }
            //DBCAST
        case 9: {
            char addOrDel[1];
            getBytes(&addOrDel[0], 1);
            std::vector<char> fileNameVec = getBytesUntilDelimeter();
            int size = fileNameVec.size() + 3;
            char fileNameArr[fileNameVec.size()];
            encoderDecoder->vectorToArray(fileNameVec, fileNameArr);
            char *packetToEncode = new char[size];
            mergeArrays(packetToEncode, opCodeArr, 2, 0);
            mergeArrays(packetToEncode, addOrDel, 1, 2);
            mergeArrays(packetToEncode, fileNameArr, fileNameVec.size(), 3);


            packetFromServer = encoderDecoder->decodeBytes(packetToEncode, size);
            break;
        }

        default:
            std::cout << ("something went wrong") << std::endl;
            break;
    }

    return packetFromServer;
}


void ConnectionHandler::mergeArrays(char *insertTo, char *insertFrom, int sizeOfInsertFrom, int from) {
    for (int i = from; i < from + sizeOfInsertFrom; i++) {
        insertTo[i] = insertFrom[i - from];
    }

}


std::vector<char> ConnectionHandler::getBytesUntilDelimeter() {
    char byte[1];
    std::vector<char> msg;
    getBytes(&byte[0], 1);
    while (byte[0] != '\0') {
        msg.push_back(byte[0]);
        getBytes(&byte[0], 1);
    }
    msg.push_back(byte[0]);
    return msg;
}

std::string ConnectionHandler::getFileName() {
    return encoderDecoder->getFileName();
}


void ConnectionHandler::updateCurrentAction(char *bytes) {

    _currentAction = encoderDecoder->bytesToShort(bytes[0], bytes[1]);
    if (_currentAction == 4) {
        _currentAction = 1;
    }


}

bool ConnectionHandler::isLoggedIn(){
    return loggedIn;
}
