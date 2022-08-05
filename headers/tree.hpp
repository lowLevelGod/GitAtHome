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
        const std::vector<char> getTreeEntryString();
        const std::string& getFileName() const{ return fileName;}
        TreeEntry(const std::string&, const Object&);
};

class Tree : public Object
{
    private:
        const std::string getHeader(const size_t) const override;
    public:
        void serialize(const std::string&) const override;
        void print() const override;
        Tree(std::vector<TreeEntry>&);
        Tree() : Object() {}
        ~Tree() override = default;
};