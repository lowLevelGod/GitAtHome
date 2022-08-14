#pragma once
#include <../headers/object.hpp>
#include <../headers/tree.hpp>
#include <memory>

class CommitMessage;

class Commit : public Object
{
    private:
        std::shared_ptr<Tree> tree;
        const std::string getHeader(const size_t) const override;
    public:
        void serialize(const std::string&) const override;
        void print() const override;
        Commit(const Tree&, const CommitMessage&, const std::vector<std::string>&);
        explicit Commit(const std::shared_ptr<Tree>& tree) : tree(tree) {}
        Commit() = default;
        ~Commit() override = default;
};

class CommitMessage
{   
    friend Commit;
    private:
        std::string author;
        std::string committer;
        std::string message;
    public:
        CommitMessage(const std::string& author, const std::string& committer, const std::string& message) 
        : author{author}, committer{committer}, message{message} {}
};