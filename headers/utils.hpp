#pragma once
#include <string>
#include <vector>
#include <memory>
#include <object.hpp>
#include <blob.hpp>
#include <tree.hpp>
#include <commit.hpp>
#include <sys/stat.h>
#include <map>

class Utils
{
    private:
        static const std::shared_ptr<Blob> parseBlob(const std::vector<uint8_t>&);
        static const std::shared_ptr<Tree> parseTree(const std::vector<uint8_t>&);
        static const std::shared_ptr<Commit> parseCommit(const std::vector<uint8_t>&);
    public:
        static const std::string getSHA1hash(const std::vector<uint8_t>&);
        static const std::vector<uint8_t> readBinaryFile(const std::string&);
        static const std::vector<uint8_t> decompressBytes(const std::vector<uint8_t>& ,const size_t);
        static const std::string getMode(const std::string&);
        static const std::vector<uint8_t> int32ToBytes(const int );
        static const uint32_t bytesToInt32(const std::vector<uint8_t>&);
        static const std::vector<uint8_t> packStringToPackedBytes(const std::string&);
        static const std::string unpackBytesToString(const std::vector<uint8_t>&);
        static void writeBinaryFile(const std::string&, std::vector<uint8_t>&);
        static const std::vector<std::string> listAllFiles(const std::string&); 
        static const bool fileExists(const std::string&);
        static const std::vector<std::string> splitPath(std::string);
        static const std::shared_ptr<Object> parseObjectFile(const std::string&);
        static const std::string getPathFromHash(const std::string&);
        static const std::map<std::string, struct stat> getStatsInDir(const std::string&);
        static bool isDir(const std::string&);
        static const std::string getModeFromStat(const struct stat&);
};