#pragma once
#include <string>
#include <vector>

class Utils
{
    public:
        static const std::string getSHA1hash(const std::vector<char>&);
        static const std::vector<char> readBinaryFile(const std::string&);
        static const std::vector<char> decompressObject(const std::string& ,const size_t);
        static const std::string getMode(const std::string&);
        static const std::vector<char> int32ToBytes(const int );
        static const uint32_t bytesToInt32(const std::vector<char>&);
        static const std::vector<char> packStringToPackedBytes(const std::string&);
        static const std::string unpackBytesToString(const std::vector<char>&);
        static void writeBinaryFile(const std::string&, const std::vector<char>&);
        static const std::vector<std::string> listAllFiles(const std::string&); 
        static const bool fileExists(const std::string&);
};