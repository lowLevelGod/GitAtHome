#include <../headers/ref.hpp>
#include <../headers/utils.hpp>
#include <fstream>
#include <iostream>

void Ref::updateHead(const std::string& commitHash)
{
    std::vector<char> v;
    v.assign(commitHash.begin(), commitHash.end());
    
    Utils::writeBinaryFile(".gitAtHome/HEAD", v);
}

const std::string Ref::getHead()
{
    std::string result;
    std::vector<char> temp = Utils::readBinaryFile(".gitAtHome/HEAD");
    result.assign(temp.begin(), temp.end());

    return result;
}
