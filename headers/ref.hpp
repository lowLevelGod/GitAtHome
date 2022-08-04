#pragma once
#include <string>

class Ref
{
    public:
        static void updateHead(const std::string&);
        static const std::string getHead();
};