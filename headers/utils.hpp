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
};