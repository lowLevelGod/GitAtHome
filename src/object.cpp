#include <../headers/object.hpp>
#include <../headers/utils.hpp>
#include <../headers/git.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>

const std::string Object::getHeader(const size_t len) const
{
    std::string result = "";
    result += " ";
    result += std::to_string(len);
    result += '\0';

    return result;
}

void Object::serialize(const std::string& header) const
{
    struct stat st = {0};

    if (Utils::fileExists(Utils::getPathFromHash(hash))) { // check if object exists
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

    Utils::writeBinaryFile(Utils::getPathFromHash(hash), compressedResult);
}