#pragma once
#include <../headers/object.hpp>
#include <../headers/tree.hpp>

class Commit : public Object
{
    private:
        Tree tree;
        std::string author;
        std::string committer;
        std::string message;
        const std::string getHeader(const size_t) override;
    public:
        void serialize() override;
        void print() override;
        ~Commit() override = default;
};