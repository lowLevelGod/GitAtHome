#pragma once
#include <../headers/object.hpp>
#include <memory>


class TreeEntry
{
    private:
        std::string mode;
        std::string fileName;
        std::string hash;
    public:
        const std::string& getMode() const { return mode;}
        const std::string& getHash() const { return hash;}
        const std::vector<uint8_t> getTreeEntryString();
        const std::string& getFileName() const{ return fileName;}
        TreeEntry() = default;
        TreeEntry(const TreeEntry& t)
         : mode(t.mode), fileName(t.fileName), hash(t.hash) {} 
        TreeEntry(const std::string&, const Object&);
        TreeEntry(const std::string& mode, const std::string& fileName, const std::string& hash) 
        : mode(mode), fileName(fileName), hash(hash) {}
};

class Tree : public Object
{
    private:
        const std::string getHeader(const size_t) const override;
    public:
        void serialize(const std::string&) const override;
        void print() const override;
        explicit Tree(std::vector<TreeEntry>&);
        Tree() : Object() {}
        ~Tree() override = default;
};