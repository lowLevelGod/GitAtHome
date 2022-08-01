#pragma once
#include <../headers/object.hpp>
#include <../headers/tree.hpp>
#include <memory>

class Commit : public Object
{
    class CommitMessage
    {   
        friend Commit;
        private:
            std::string author;
            std::string committer;
            std::string message;
    };
    private:
        const std::string getHeader(const size_t) override;
    public:
        void serialize(const std::string&) override;
        void print() override;
        Commit(const Tree&, const CommitMessage&, const std::vector<std::shared_ptr<Commit>>&);
        ~Commit() override = default;
};