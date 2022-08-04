#include <../headers/ref.hpp>
#include <../headers/utils.hpp>
#include <fstream>
#include <iostream>

void Ref::updateHead(const std::string& commitHash)
{
    std::ofstream file(".gitAtHome/HEAD", std::ios::binary);
    file.write(commitHash.c_str(), commitHash.length());
    file.close();
    if (!file)
        std::cout << "Write failed!" << std::endl;
}

const std::string Ref::getHead()
{
    std::string result;
    std::vector<char> temp = Utils::readBinaryFile(".gitAtHome/HEAD");
    result.assign(temp.begin(), temp.end());

    return result;
}
