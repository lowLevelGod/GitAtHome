#pragma once
#include <string>
#include <tree.hpp>
#include <map>

class Ref
{
    public:
        static std::map<std::string, TreeEntry> headEntries;
        static void updateHead(const std::string&);
        static const std::string getHead();
};