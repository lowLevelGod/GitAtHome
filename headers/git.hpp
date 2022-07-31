#pragma once
#include <vector>
#include <string>

class Git
{             
    public:
        static void run();
        static void init();
        static const std::string getSHA1hash(const std::vector<char>&);
        static const std::vector<char> readBinaryFile(const std::string&);
};