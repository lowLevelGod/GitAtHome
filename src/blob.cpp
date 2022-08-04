#include <../headers/blob.hpp>
#include <iostream>
#include <iterator>
#include <../headers/utils.hpp>

const std::string Blob::getHeader(const size_t len) const
{
    return "blob" + Object::getHeader(len);
}

void Blob::serialize(const std::string& t = "") const
{
    std::string header = getHeader(content.size());
    Object::serialize(header);
}

Blob::Blob(const std::string& fileName)
{
    // read file content
    content = Utils::readBinaryFile(fileName);
    // create header
    std::string header = getHeader(content.size());

    // compute SHA1 hash of header + content 
    std::vector<char> hashedContent(header.begin(), header.end());
    hashedContent.insert(hashedContent.end(), content.begin(), content.end());
    hash.assign(Utils::getSHA1hash(hashedContent));
}
 
void Blob::print() const
{   
    std::vector<char> decompressedResult = Utils::decompressObject(getPath() ,
                                                                getHeader(content.size()).length() + content.size() + 1);
    for (auto x : decompressedResult)
        std::cout << x;
    std::cout << std::endl;
}