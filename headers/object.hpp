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
        virtual const std::string getHeader(const size_t) const;
    public:
        const std::string& getHash() const { return hash;}
        virtual void serialize(const std::string&) const; // DO NOT LIKE THIS USELESS PARAMETER AT ALL. HAVE TO FIX LATER
        virtual void print() const = 0;
        Object() : content(std::vector<char>()), hash("") {}
        explicit Object(const std::vector<char>& content) : content(content) {}
        virtual ~Object() = default;

};