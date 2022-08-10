#pragma once
#include <string>
#include <vector>
#include <memory>
#include <object.hpp>
#include <blob.hpp>
#include <tree.hpp>
#include <commit.hpp>

class Utils
{
    public:
        static const std::string getSHA1hash(const std::vector<char>&);
        static const std::vector<char> readBinaryFile(const std::string&);
        static const std::vector<char> decompressBytes(const std::vector<char>& ,const size_t);
        static const std::string getMode(const std::string&);
        static const std::vector<char> int32ToBytes(const int );
        static const uint32_t bytesToInt32(const std::vector<char>&);
        static const std::vector<char> packStringToPackedBytes(const std::string&);
        static const std::string unpackBytesToString(const std::vector<char>&);
        static void writeBinaryFile(const std::string&, const std::vector<char>&);
        static const std::vector<std::string> listAllFiles(const std::string&); 
        static const bool fileExists(const std::string&);
        static const std::vector<std::string> splitPath(std::string);
        static const std::shared_ptr<Object> parseObjectFile(const std::string&);
        static const std::shared_ptr<Blob> parseBlob(const std::vector<char>&);
        static const std::shared_ptr<Tree> parseTree(const std::vector<char>&);
        static const std::shared_ptr<Commit> parseCommit(const std::vector<char>&);
        static const std::string getPathFromHash(const std::string&);
};