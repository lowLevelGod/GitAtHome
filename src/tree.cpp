#include <../headers/tree.hpp>
#include <../headers/git.hpp>
#include <iostream>
#include <algorithm>

const std::string Tree::getHeader(const size_t len)
{
    return "tree" + Object::getHeader(len);
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

Tree::Tree(std::vector<TreeEntry>& entries)
{   
    // sort tree entries after file name/folder name
    std::sort(entries.begin(), entries.end(), []
    (const TreeEntry& first, const TreeEntry& second){
        return first.getFileName() < second.getFileName();
    });

    std::vector<char> result;
    for (auto e : entries)
    {
        std::vector<char> entryString = e.getTreeEntryString();
        content.insert(content.end(), entryString.begin(), entryString.end());
    }
    std::string header = getHeader(content.size());
    result.assign(header.begin(), header.end());
    result.insert(result.end(), content.begin(), content.end());

    hash.assign(Git::getSHA1hash(result));
}


void Tree::serialize(const std::string& t = "")
{
    std::string header = getHeader(content.size());
    Object::serialize(header);
}

void Tree::print()
{
    std::vector<char> decompressedResult = Git::decompressObject(getPath() ,
                                                                getHeader(content.size()).length() + content.size() + 1);
    for (auto x : decompressedResult)
        std::cout << x;
    // std::cout << std::endl;
}