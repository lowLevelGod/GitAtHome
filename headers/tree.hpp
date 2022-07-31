#pragma once
#include <../headers/object.hpp>
#include <memory>

class Tree : public Object
{
    class TreeEntry
    {
        private:
            uint64_t mode;
            std::string fileName;
            std::string hash;
        public:
            const std::vector<char> getTreeEntryString();
    };

    private:
        const std::string getHeader(const size_t) override;
        void serialize() override;
    public:
        void print() override;
        Tree(const std::vector<TreeEntry>&);
        ~Tree() override = default;
};