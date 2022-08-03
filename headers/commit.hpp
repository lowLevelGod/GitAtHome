#pragma once
#include <../headers/object.hpp>
#include <../headers/tree.hpp>
#include <memory>

class CommitMessage;

class Commit : public Object
{
    private:
        const std::string getHeader(const size_t) const override;
    public:
        void serialize(const std::string&) const override;
        void print() const override;
        Commit(const Tree&, const CommitMessage&, const std::vector<std::shared_ptr<Commit>>&);
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