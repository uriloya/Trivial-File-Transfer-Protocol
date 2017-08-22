//
// Created by ניר דוניץ on 16.1.2017.
//


#include "../include/Packets/BasePacket.h"
#include "../include/Packets/ACKPacket.h"
#include "../include/Packets/ERRORPacket.h"
#include "../include/BidiEncoderDecoder.h"
#include "../include/Packets/BCASTPacket.h"


#include <sstream>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>


using namespace std;


BidiEncoderDecoder::BidiEncoderDecoder() :
        _opCode(0),
        _block(0),
        _packetSize(0),
        _counterRead(0), fileName() {
}

BidiEncoderDecoder::BidiEncoderDecoder(const BidiEncoderDecoder &encodeDecode) : _opCode(encodeDecode._opCode),
                                                                                 _block(encodeDecode._block),
                                                                                 _packetSize(encodeDecode._packetSize),
                                                                                 _counterRead(
                                                                                         encodeDecode._counterRead),
                                                                                 fileName(encodeDecode.fileName) {}

BasePacket *BidiEncoderDecoder::decodeBytes(char *bytes, int lengthOfArray) {
    //char opCodearr;
    short op = bytesToShort(bytes[0], bytes[1]);
    switch (op) {
        //DATA
        case 3: {

            short packetSize = bytesToShort(bytes[2], bytes[3]);
            short block = bytesToShort(bytes[4], bytes[5]);
            return new DATAPacket(op,
                                  packetSize,
                                  block,
                                  getPartOfByteArray(bytes, 6, lengthOfArray));
        }
            //ACK
        case 4: {
            short block = bytesToShort(bytes[2], bytes[3]);
            return new ACKPacket(block);
        }
            //ERROR
        case 5: {

            short errCode = _packetSize = bytesToShort(bytes[2], bytes[3]);
            //length until 0.
            return new ERRORPacket(errCode, bytesToString(getPartOfByteArray(bytes, 4, lengthOfArray - 1)));
        }
            //BCAST
        case 9: {
            std::string fileName = bytesToString(getPartOfByteArray(bytes, 3, lengthOfArray));
            //addor del
            return new BCASTPacket(bytes[2], fileName);

        }
        default:
            return nullptr;
    }

}


std::string BidiEncoderDecoder::bytesToString(char bytes[]) {
    //return std::string(bytes);
    return std::string(bytes);


}


char *BidiEncoderDecoder::getPartOfByteArray(char bytes[], int from, int to) {
    int size = to - from;
    char *newArray = new char[size];
    for (int i = from; i < to; i++) {
        newArray[i - from] = bytes[i];
    }
    return newArray;
}


void BidiEncoderDecoder::shortToBytes(short num, char *bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}


bool BidiEncoderDecoder::fileExist(string name) {

    string tmpName = "./" + name;

    ifstream ifStream;
    ifStream.open(tmpName);

    bool success = ifStream.is_open();
    if (success)
        ifStream.close();
    return success;
}

std::vector<char> BidiEncoderDecoder::encodeInputTobytes(std::string line) {
    std::vector<std::string> lineSplited;
    boost::split(lineSplited, line, boost::is_any_of(" "));

    char opCodeBytes[2];
    std::vector<char> bytesVec;

    vector<string>::iterator it;

    //line splited.at(0)
    it = lineSplited.begin();
    string request = it->data();
    string str;
    if (lineSplited.size() > 1) {
        it++;
        //line splited.at(1)
        str = it->data();
    }
    if (request == "RRQ") {


        shortToBytes((short) 1, opCodeBytes);
        arrayToVector(&bytesVec, opCodeBytes, 2);
        this->fileName = str;
        //inssert file name as chars
        std::copy(fileName.begin(), fileName.end(), std::back_inserter(bytesVec));
        bytesVec.push_back('\0');

    } else if (request == "WRQ") {
        if (fileExist(str)&&lineSplited.size()>1) {
            this->fileName = str;
            shortToBytes((short) 2, opCodeBytes);

            arrayToVector(&bytesVec, opCodeBytes, 2);
            std::copy(fileName.begin(), fileName.end(), std::back_inserter(bytesVec));
            bytesVec.push_back('\0');
        }else{
        }

    } else if (request == "ACK") {
        shortToBytes((short) 4, opCodeBytes);
        arrayToVector(&bytesVec, opCodeBytes, 2);
        short block = boost::lexical_cast<short>(str);
        char blockNumArr[2];
        shortToBytes(block, blockNumArr);
        arrayToVector(&bytesVec, blockNumArr, 2);

//        std::copy(str.begin(), str.end(), std::back_inserter(bytesVec));

    } else if (request == "DIRQ") {
        shortToBytes((short) 6, opCodeBytes);
        arrayToVector(&bytesVec, opCodeBytes, 2);

    } else if (request == "LOGRQ") {
        shortToBytes((short) 7, opCodeBytes);
        arrayToVector(&bytesVec, opCodeBytes, 2);
        std::copy(str.begin(), str.end(), std::back_inserter(bytesVec));
        bytesVec.push_back('\0');
    } else if (request == "DELRQ") {
        this->fileName = str;
        shortToBytes((short) 8, opCodeBytes);
        arrayToVector(&bytesVec, opCodeBytes, 2);
        std::copy(fileName.begin(), fileName.end(), std::back_inserter(bytesVec));
        bytesVec.push_back('\0');
    } else if (request == "DISC") {
        shortToBytes((short) 10, opCodeBytes);
        arrayToVector(&bytesVec, opCodeBytes, 2);

    } else {
    }
    return bytesVec;

}

//void BidiEncoderDecoder::addStringToBytes(std::string str, char *bytes, int from) {
//    char temp[str.size() + 1];
//    strcpy(temp, str.c_str());
//    for (int i = from; i - from < (sizeof(temp) / sizeof(*temp)); i++) {
//        bytes[i] = temp[i - from];
//    }
//}

std::string BidiEncoderDecoder::getFileName() {
    return this->fileName;
}

void BidiEncoderDecoder::arrayToVector(std::vector<char> *v, char *arr, int size) {
    for (int i = 0; i < size; i++) {
        (*v).push_back(arr[i]);
    }
}


void BidiEncoderDecoder::vectorToArray(std::vector<char> vector, char *arr) {
    int i = 0;
    for (std::vector<char>::iterator it = vector.begin(); it != vector.end(); ++it) {

        arr[i] = it.operator*();
        i++;
    }

}

short BidiEncoderDecoder::bytesToShort(char a, char b) {
    short result = (short) ((a & 0xff) << 8);
    result += (short) (b & 0xff);
    return result;
}

BidiEncoderDecoder::~BidiEncoderDecoder() {

}

