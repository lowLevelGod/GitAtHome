#pragma once
#include <object.hpp>
#include <sys/stat.h>

class IndexEntry
{
    private:
        struct stat st;
        std::string hash;
        uint16_t flags;
        std::string fileName;
        // remembert to add padding for 8 byte alignment
    public:

};

class Index : public Object
{
    private:
        const std::string getHeader(const size_t) const;
    public:
        void serialize(const std::string&) const;
        void print() const;
};