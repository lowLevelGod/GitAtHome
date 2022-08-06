#include <../headers/object.hpp>
#include <../headers/utils.hpp>
#include <../headers/git.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>

const std::string Object::getPath() const
{
    std::string pathDir = Git::gitDir + "/" + "objects" + "/" + hash.substr(0, 2);

    struct stat st = {0};

    if (stat(pathDir.c_str(), &st) == -1) { // check if directory exists
        mkdir(pathDir.c_str(), 0777); // testing purposes
    }

    return Git::gitDir + "/" + "objects" + "/" + hash.substr(0, 2) + "/" + hash.substr(2);
}

const std::string Object::getHeader(const size_t len) const
{
    std::string result = "";
    result += " ";
    result += std::to_string(len);
    result += "\0";

    return result;
}

void Object::serialize(const std::string& header) const
{
    struct stat st = {0};

    if (stat(getPath().c_str(), &st) != -1) { // check if object exists
        // don't create object twice
        return;
    }

    // append blob content to header
    std::vector<char> result(header.begin(), header.end());
    result.insert(result.end(), content.begin(), content.end());

    uLong ucompSize = result.size() + 1;
    uLong compSize = compressBound(ucompSize);
    std::vector<char> compressedResult(compSize); // allocate enough size for output buffer
    // Deflate
    try
    {
        compress((Bytef *)compressedResult.data(), &compSize, (Bytef *)result.data(), ucompSize);
    }
    catch(const std::exception& e)
    {
        // std::cerr << e.what() << '\n';
        std::cout << "Compression failed!" << std::endl;
    }
    //resize vector to actual size of compressed data
    compressedResult.resize(compSize);

    Utils::writeBinaryFile(getPath(), compressedResult);
}