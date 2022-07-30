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
            std::shared_ptr<Object> object;
            const std::string& getTreeEntryString();
    };

    private:
        const std::string getHeader(const size_t) override;
        void serialize() override;
    public:
        void print() override;
        ~Tree() override = default;
};