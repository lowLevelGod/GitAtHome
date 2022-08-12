#include <../headers/tree.hpp>
#include <../headers/utils.hpp>
#include <iostream>
#include <algorithm>

const std::string Tree::getHeader(const size_t len) const
{
    return "tree" + Object::getHeader(len);
}

const std::vector<uint8_t> TreeEntry::getTreeEntryString()
{
    std::string prefix = "";
    prefix += mode;
    prefix += fileName;
    prefix += '\0';

    std::vector<uint8_t> result;
    result.assign(prefix.begin(), prefix.end());

    // turn SHA1 into 20 bytes
    std::vector<uint8_t> packedSHA = Utils::packStringToPackedBytes(hash);
    result.insert(result.end(), packedSHA.begin(), packedSHA.end());
    
    return result;
}

TreeEntry::TreeEntry(const std::string& fileName, const Object& obj) : fileName{fileName} 
{
    mode = Utils::getMode(fileName);
    obj.serialize("");
    hash = obj.getHash();
}

Tree::Tree(std::vector<TreeEntry>& entries)
{   
    // sort tree entries after file name/folder name
    // std::sort(entries.begin(), entries.end(), []
    // (const TreeEntry& first, const TreeEntry& second){
    //     return first.getFileName() < second.getFileName();
    // });

    std::vector<uint8_t> result;
    for (auto e : entries)
    {
        std::vector<uint8_t> entryString = e.getTreeEntryString();
        content.insert(content.end(), entryString.begin(), entryString.end());
    }
    std::string header = getHeader(content.size());
    result.assign(header.begin(), header.end());
    result.insert(result.end(), content.begin(), content.end());

    hash.assign(Utils::getSHA1hash(result));
}


void Tree::serialize(const std::string& t = "") const
{
    std::string header = getHeader(content.size());
    Object::serialize(header);
}

void Tree::print() const
{
   
}