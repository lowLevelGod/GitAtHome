#pragma once
#include <vector>
#include <string>
#include <tree.hpp>

class Git
{             
    public:
        static void run();
        static void init();
        static void commit();
        static const Tree createCommitTree(const std::string&);
};