#include <../headers/blob.hpp>
#include <iostream>
#include <iterator>
#include <../headers/git.hpp>

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
    // read file content
    content = Git::readBinaryFile(fileName);
    // create header
    std::string header = getHeader(content.size());

    // compute SHA1 hash of header + content 
    std::vector<char> hashedContent(header.begin(), header.end());
    hashedContent.insert(hashedContent.end(), content.begin(), content.end());
    hash.assign(Git::getSHA1hash(hashedContent));
}

void Blob::print()
{
    // read compressed blob from objects
    std::vector<char> compressed(Git::readBinaryFile(getPath()));

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