#pragma once
#include <../headers/object.hpp>

class Blob : public Object
{
    private:
        const std::string getHeader(const size_t) override;
    public:
        void serialize(const std::string&) override;
        void print() override;
        Blob(const std::string&);
        ~Blob() override = default;
};