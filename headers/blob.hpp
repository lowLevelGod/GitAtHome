#pragma once
#include <../headers/object.hpp>

class Blob : public Object
{
    private:
        const std::string getHeader(const size_t) const override;
    public:
        void serialize(const std::string&) const override;
        void print() const override;
        explicit Blob(const std::string&);
        explicit Blob(const std::vector<uint8_t>&);
        ~Blob() override = default;
};