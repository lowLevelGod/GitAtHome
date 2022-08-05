#include <../headers/index.hpp>
#include <../headers/utils.hpp>

const std::string Index::getHeader(const size_t len) const
{
    std::string result = "DIRC";
    result +=  "\0\0\0\0";

    // big-Endian representation of number of entries
    std::vector<char> lenBytes(Utils::int32ToBytes(len));
    result.insert(result.end(), lenBytes.begin(), lenBytes.end());

    return result;
}

void Index::serialize(const std::string&) const
{
    // TODO
}

void Index::print() const
{
    // TODO
}