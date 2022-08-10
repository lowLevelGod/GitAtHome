#pragma once
#include <object.hpp>
#include <sys/stat.h>
#include <memory>
#include <map>
#include <utils.hpp>
#include <set>

class IndexEntry
{
    private:
        struct stat st;
        std::string hash;
        uint16_t flags;
        std::string fileName;
        std::shared_ptr<Object> obj;
        // remembert to add padding for 8 byte alignment
    public:
        const std::string& getFileName() const { return fileName;}
        const std::string& getHash() const { return hash;}
        const std::string getMode() const { return Utils::getMode(fileName);}
        const std::vector<char> getIndexEntryString() const;
        IndexEntry() = default;
        explicit IndexEntry(const std::string&);
        IndexEntry(const struct stat& st, const std::string& hash, const uint16_t flags, const std::string& fileName)
         : st(st), hash(hash), flags(flags), fileName(fileName), obj(nullptr) {}
        IndexEntry(const IndexEntry& ie) 
        : st(ie.st), hash(ie.hash), flags(ie.flags), fileName(ie.fileName), obj(ie.obj) {}
        IndexEntry& operator=(const IndexEntry&);
};

class Index : public Object
{
    private:
        std::map<std::string, std::set<std::string>> directories;
        std::map<std::string, IndexEntry> entries;
        bool changed;
        const std::string getHeader(const size_t) const;
        bool checkSignature(const std::string&);
        void prepareSerialize();
        void serialize(const std::string&) const;
        void parseIndexFile();
        void discardConflicts(const std::string&);
        void removeEntry(const std::string&);
        void removeChildren(const std::string&);
    public:
        void add(const std::vector<std::string>&);
        void print() const;
        void saveUpdates();
        const std::vector<std::string> getFilePaths();
        const std::vector<IndexEntry> getEntries();
        Index();
};