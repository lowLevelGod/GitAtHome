#pragma once
#include <object.hpp>
#include <sys/stat.h>
#include <memory>
#include <set>

class IndexEntry
{
    private:
        std::shared_ptr<Object> obj;
        struct stat st;
        std::string hash;
        uint16_t flags;
        std::string fileName;
        // remembert to add padding for 8 byte alignment
    public:
        const std::string& getFileName() const { return fileName;}
        const std::vector<char> getIndexEntryString() const;
        explicit IndexEntry(const std::string&);
};

class Index : public Object
{
    struct cmp 
    {
        bool operator() (const IndexEntry& a, const IndexEntry& b) const  { return a.getFileName() < b.getFileName(); }
    };

    private:
        std::set<IndexEntry, cmp> entries;
        const std::string getHeader(const size_t) const;
        bool checkSignature(const std::string&);
    public:
        void parseIndexFile();
        void add(const std::vector<std::string>&);
        void prepareSerialize();
        void serialize(const std::string&) const;
        void print() const;
};