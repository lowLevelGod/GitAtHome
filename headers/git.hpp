#pragma once
#include <vector>
#include <string>
#include <tree.hpp>
#include <set>

class Git
{             
    public:
        static const std::set<std::string> ignoredDirs;
        static const std::string gitDir;
        static void run();
        static void init();
        static void commit();
        static const Tree createCommitTree(const std::string&);
};