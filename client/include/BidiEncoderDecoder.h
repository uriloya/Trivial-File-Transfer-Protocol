
#pragma once
#ifndef BIDIENCODERDECODER_H
#define BIDIENCODERDECODER_H


#include <map>
#include "Packets/BasePacket.h"
#include "Packets/DATAPacket.h"
#include "Packets/ERRORPacket.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

class BidiEncoderDecoder {
private:
    short _opCode;
    short _packetSize;
    short _block;
    int _counterRead;
    std::string fileName;

    void shortToBytes(short num, char *bytesArr);

    char *getPartOfByteArray(char bytes[], int from, int to);

    BidiEncoderDecoder(const BidiEncoderDecoder &encodeDecode);

    bool fileExist(std::string name);

public:
    virtual ~BidiEncoderDecoder();

    BidiEncoderDecoder();

    std::string getFileName();

    void vectorToArray(std::vector<char> vector, char *arr);

    BasePacket *decodeBytes(char *bytes, int lengthOfArray);

    short getOpCode(char a, char b);

    short bytesToShort(char a, char b);

    std::string bytesToString(char *bytes);

    void arrayToVector(std::vector<char> *v, char *arr, int size);

    std::vector<char> encodeInputTobytes(std::string line);


};

#endif //BIDIENCODERDECODER_H
