#include <../headers/tree.hpp>
#include <iostream>

const std::string getHeader(const size_t len)
{
    std::string result = "";
    result += "tree";
    result += " ";
    result += std::to_string(len);
    result += "\0";

    return result;
}

const std::vector<char> Tree::TreeEntry::getTreeEntryString()
{
    std::string prefix = "";
    prefix += std::to_string(mode);
    prefix += fileName;
    prefix += "\0";

    std::vector<char> result;
    result.assign(prefix.begin(), prefix.end());

    // turn SHA1 into 20 bytes
    std::string tempHash = hash;
    for (size_t i = 0, len = tempHash.length(); i < len - 1; i += 2)
    {
        uint8_t b1 = 0, b2 = 0;
        if (tempHash[i] >= '0' && tempHash[i] <= '9')
            b1 = tempHash[i] - '0';
        else
            b1 = tempHash[i] - 'a' + 10;
        if (tempHash[i + 1] >= '0' && tempHash[i + 1] <= '9')
            b2 = tempHash[i + 1] - '0';
        else
            b2 = tempHash[i + 1] - 'a' + 10;
        
        result.push_back(
            ((b1  & 0xf) << 4)
            | (b2  & 0xf) 
        );          
    }

    return result;
}

Tree::Tree(const std::vector<TreeEntry>& entries)
{
    for (auto e : entries)
    {
        std::vector<char> entryString = e.getTreeEntryString();
        content.insert(content.end(), entryString.begin(), entryString.end());
    }
}


void Tree::serialize()
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