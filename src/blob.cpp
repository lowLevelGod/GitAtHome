#include <../headers/blob.hpp>
#include <iostream>
#include <iterator>

const std::string Blob::getHeader(const size_t len)
{
    std::string header = "";
    header += "blob";
    header += " ";
    header += std::to_string(len);
    header += "\0";

    return header;
}

void Blob::serialize()
{
    std::string header = getHeader(content.size());
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
    std::ofstream file(getPath(), std::ios::binary);
   
    file.write(compressedResult.data(), compressedResult.size());
    
    file.close();
    if (!file)
        std::cout << "Write failed!" << std::endl;
}

Blob::Blob(const std::string& fileName)
{
    std::ifstream file(fileName, std::ios::binary);

    file.unsetf(std::ios::skipws); // for newlines

    // get file size
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    // std::cout << fileSize << std::endl;
    std::string header = getHeader(fileSize);

    content.reserve(fileSize);

    content.insert(content.begin(),
               std::istream_iterator<char>(file),
               std::istream_iterator<char>());
    // std::cerr << "Error: " << strerror(errno);
    file.close();
    if (!file)
        std::cout << "Read failed!" << std::endl; // Prints it every time for some reason

    // compute SHA1 hash of header + content 
    std::vector<char> hashedContent(header.begin(), header.end());

    char hex[SHA1_HEX_SIZE]; // temporary buffer for sha1 hash
    hashedContent.insert(hashedContent.end(), content.begin(), content.end());
        sha1(hashedContent.data())
        // finalize must be called, otherwise the hash is not valid
        // after that, no more bytes should be added
        .finalize()
        // print the hash in hexadecimal, 0-terminated
        .print_hex(hex);
    hash.assign(hex);

}

void Blob::print()
{
    std::ifstream file(getPath(), std::ios::binary);

    // get file size
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> compressed(fileSize);
    compressed.insert(compressed.begin(),
               std::istream_iterator<char>(file),
               std::istream_iterator<char>());// read compressed blob from objects

    uLong ucompSize = getHeader(content.size()).length() + content.size() + 1;
    uLong compSize = compressBound(ucompSize);
    std::vector<char> decompressedResult(ucompSize); // allocate enough size for output buffer
    // Inflate
    try
    {
        uncompress((Bytef *)decompressedResult.data(), &ucompSize, (Bytef *)compressed.data(), compSize);
    }
    catch(const std::exception& e)
    {
        // std::cerr << e.what() << '\n';
        std::cout << "Decompression failed!" << std::endl;
    }
    
    for (auto x : decompressedResult)
        std::cout << x;
    // std::cout << std::endl;
}