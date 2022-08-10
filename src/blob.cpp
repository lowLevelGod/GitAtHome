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
    // std::cout << "Actual size of content: " << content.size() << std::endl;
    // create header
    std::string header = getHeader(content.size());

    // compute SHA1 hash of header + content 
    std::vector<char> hashedContent(header.begin(), header.end());
    hashedContent.insert(hashedContent.end(), content.begin(), content.end());
    hash.assign(Utils::getSHA1hash(hashedContent));
}

Blob::Blob(const std::vector<char>& content) : Object(content)
{
    std::string header = "blob " + std::to_string(content.size()) + '\0';
    std::vector<char> result(header.begin(), header.end());
    result.insert(result.end(), content.begin(), content.end());
    hash = Utils::getSHA1hash(result);
}
 
void Blob::print() const
{   
    
}