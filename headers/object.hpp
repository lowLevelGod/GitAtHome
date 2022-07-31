#pragma once
#include <string>
#include <fstream>
#include <zlib.h>
#include <vector>

class Object
{
    protected:
        std::vector<char> content;
        std::string hash;
        virtual const std::string getHeader(const size_t) = 0;
    public:
        const std::string& getHash() { return hash;}
        const std::string getPath();
        virtual void serialize() = 0;
        virtual void print() = 0;
        virtual ~Object() = default;

};