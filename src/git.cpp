#include <../headers/git.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sha1.hpp>
#include <blob.hpp>

void Git::init()
{
    mkdir("./.gitAtHome", 0777);
    mkdir("./.gitAtHome/objects", 0777);
}  

const std::string Git::getSHA1hash(const std::vector<char>& content)
{
    char hex[SHA1_HEX_SIZE]; // temporary buffer for sha1 hash
    sha1(content.data())
    // finalize must be called, otherwise the hash is not valid
    // after that, no more bytes should be added
    .finalize()
    // print the hash in hexadecimal, 0-terminated
    .print_hex(hex);

    return std::string(hex);
}

const std::vector<char> Git::readBinaryFile(const std::string& fileName)
{
    std::vector<char> result;
    std::ifstream file(fileName, std::ios::binary);

    file.unsetf(std::ios::skipws); // for newlines
    // get file size
    std::streampos fileSize;
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    result.reserve(fileSize);

    result.insert(result.begin(),
               std::istream_iterator<char>(file),
               std::istream_iterator<char>());
    
    // std::cerr << "Error: " << strerror(errno);
    file.close();
    // if (!file)
    //     std::cout << "Read failed!" << std::endl; // Prints it every time for some reason

    return result;
}

void Git::run()
{
    init();
    Blob blob("test3.txt");
    blob.serialize();
    blob.print();
}