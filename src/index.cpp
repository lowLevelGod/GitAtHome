#include <../headers/index.hpp>
#include <../headers/utils.hpp>
#include <../headers/blob.hpp>
#include <../headers/tree.hpp>
#include <../headers/git.hpp>
#include <algorithm>
#include <sys/dir.h>
#include <iostream>


IndexEntry::IndexEntry(const std::string& fileName) : fileName(fileName)
{
    if (Utils::fileExists(fileName))
    {
        std::string stringMode = Utils::getMode(fileName);
        flags = std::min(fileName.length(), static_cast<size_t>(0xfff));
        if (stringMode[0] == '4')
        {
            /* files only for now */
            // obj = std::make_shared<Tree>(Tree(std::vector<TreeEntry>()));
        }
        else 
        {
            obj = std::make_shared<Blob>(Blob(fileName));
        }

        obj->serialize("");
        hash = obj->getHash();
    }else
    {
        perror("");
    }
}

IndexEntry& IndexEntry::operator=(const IndexEntry& ie)
{
    st = ie.st;
    hash = ie.hash; 
    flags = ie.flags;
    fileName = ie.fileName;
    obj = ie.obj;

    return *this;
}

const std::vector<char> IndexEntry::getIndexEntryString() const
{
    /*32-bit ctime seconds, the last time a file's metadata changed
    32-bit ctime nanosecond fractions
    32-bit mtime seconds, the last time a file's data changed
    32-bit mtime nanosecond fractions
    32-bit dev
    32-bit ino
    32-bit mode
    32-bit uid
    32-bit gid
    32-bit file size*/
    std::vector<char> lastcTimeMetadata = Utils::int32ToBytes(st.st_ctim.tv_sec);
    std::vector<char> cTimeNano = Utils::int32ToBytes(st.st_ctim.tv_nsec);
    std::vector<char> lastmTimeData = Utils::int32ToBytes(st.st_mtim.tv_sec);
    std::vector<char> mTimeNano = Utils::int32ToBytes(st.st_mtim.tv_nsec);;
    std::vector<char> dev = Utils::int32ToBytes(st.st_dev);
    std::vector<char> ino = Utils::int32ToBytes(st.st_ino);
    std::vector<char> mode = Utils::int32ToBytes(std::stoul(Utils::getMode(fileName), nullptr, 8));
    std::vector<char> uid = Utils::int32ToBytes(st.st_uid);
    std::vector<char> gid = Utils::int32ToBytes(st.st_gid);
    std::vector<char> fileSize = Utils::int32ToBytes(st.st_size);

    std::vector<char> result;
    result.insert(result.end(), lastcTimeMetadata.begin(), lastcTimeMetadata.end());
    result.insert(result.end(), cTimeNano.begin(), cTimeNano.end());
    result.insert(result.end(), lastmTimeData.begin(), lastmTimeData.end());
    result.insert(result.end(), mTimeNano.begin(), mTimeNano.end());
    result.insert(result.end(), dev.begin(), dev.end());
    result.insert(result.end(), ino.begin(), ino.end());
    result.insert(result.end(), mode.begin(), mode.end());
    result.insert(result.end(), uid.begin(), uid.end());
    result.insert(result.end(), gid.begin(), gid.end());
    result.insert(result.end(), fileSize.begin(), fileSize.end());

    // turn SHA1 into 20 bytes
    std::vector<char> packedSHA = Utils::packStringToPackedBytes(hash);
    result.insert(result.end(), packedSHA.begin(), packedSHA.end());

    // flags
    result.push_back(static_cast<char>((flags >> 8) & 0xff));
    result.push_back(static_cast<char>((flags >> 0) & 0xff));

    // file name
    result.insert(result.end(), fileName.begin(), fileName.end());
    // pad to multiple of 8
    int padding = 8 - result.size() % 8;
    for (int i = 1; i <= padding; ++i)
        result.push_back('\0');

    return result;
}

const std::string Index::getHeader(const size_t len) const
{
    std::string result = "DIRC";
    // just ignore version number
    result += '\0';
    result += '\0';
    result += '\0';
    result += '\0';

    // big-Endian representation of number of entries
    std::vector<char> lenBytes(Utils::int32ToBytes(len));
    result.insert(result.end(), lenBytes.begin(), lenBytes.end());

    return result;
}

void Index::prepareSerialize()
{
    /* Might move this block of code later, because I don't want to construct a new Index again each time I add*/

    std::vector<char> result;
    content.clear(); // we make sure to add all elements again, not over last ones
    for (auto e : entries)
    {
        std::vector<char> entryString = e.second.getIndexEntryString();
        content.insert(content.end(), entryString.begin(), entryString.end());
    }
    std::string header = getHeader(entries.size());
    result.assign(header.begin(), header.end());
    result.insert(result.end(), content.begin(), content.end());

    hash.assign(Utils::getSHA1hash(result));
}

void Index::add(const std::vector<std::string>& files)
{
    changed = true;
    for (auto file : files)
        entries[file] = IndexEntry(file);
    
}

void Index::serialize(const std::string&) const
{
    std::string header = getHeader(entries.size());
    std::vector<char> v;
    // header
    v.assign(header.begin(), header.end());
    // actual content
    v.insert(v.end(), content.begin(), content.end());
    // SHA1 hash
    std::vector<char> packedHash = Utils::packStringToPackedBytes(hash);
    v.insert(v.end(), packedHash.begin(), packedHash.end());

    Utils::writeBinaryFile(Git::gitDir + "/" + "index", v);
}

void Index::print() const
{
   // TO DO
}

bool Index::checkSignature(const std::string& s)
{
    return (s.substr(0, 4) == "DIRC") &&
            s[4] == '\0' && s[5] == '\0' &&
            s[6] == '\0' && s[7] == '\0';
}

void Index::parseIndexFile()
{

    entries.clear(); // flush junk data
    std::vector<char> index = Utils::readBinaryFile(Git::gitDir + "/" + "index");
    if (index.size() == 0)
        return;
    if (!checkSignature(std::string(index.begin(), index.begin() + 8)))
    {
        std::cout << "Bad index signature!" << std::endl;
        return;
    }

    uint32_t entryCount = Utils::bytesToInt32(std::vector<char>(index.begin() + 8, index.begin() + 12));
    
    size_t start = 12;
    for (size_t i = 0; i < entryCount; ++i)
    {
        struct stat st;
        st.st_ctim.tv_sec = Utils::bytesToInt32(std::vector<char>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_ctim.tv_nsec = Utils::bytesToInt32(std::vector<char>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_mtim.tv_sec = Utils::bytesToInt32(std::vector<char>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_mtim.tv_nsec = Utils::bytesToInt32(std::vector<char>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_dev = Utils::bytesToInt32(std::vector<char>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_ino = Utils::bytesToInt32(std::vector<char>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_mode  = Utils::bytesToInt32(std::vector<char>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_uid = Utils::bytesToInt32(std::vector<char>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_gid = Utils::bytesToInt32(std::vector<char>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_size = Utils::bytesToInt32(std::vector<char>(index.begin() + start, index.begin() + start + 4));
        start += 4;

        std::string ieHash = Utils::unpackBytesToString(std::vector<char>(index.begin() + start, index.begin() + start + 20));
        start += 20;

        uint16_t ieFlags = (index[start] << 8) | index[start + 1];
        start += 2;

        std::string ieFileName = "";
        for (; index[start] != '\0'; ++start)
            ieFileName += index[start];

        start = start + 8 - (start - 12) % 8;
        entries[ieFileName] = IndexEntry(st, ieHash, ieFlags, ieFileName);
    }
    
    if (Utils::getSHA1hash(std::vector<char>(index.begin(), index.end() - 20))
        != Utils::unpackBytesToString(
            std::vector<char>(index.end() - 20, index.end())
            ))
                std::cout << "Checksum corrupt !" << std::endl;
}

Index::Index() : Object(), entries(std::map<std::string, IndexEntry>()), changed(false)
{
    if (Utils::fileExists(Git::gitDir + "/" + "index"))
        parseIndexFile();
}

const std::vector<std::string> Index::getFilePaths()
{
    std::vector<std::string> paths;
    for (auto e : entries)
    {
        paths.push_back(e.first);
    }

    return paths;
}

const std::vector<IndexEntry> Index::getEntries()
{
    std::vector<IndexEntry> result;
    for (auto x : entries)
        result.push_back(x.second);
        
    return result;
}

void Index::saveUpdates()
{
    prepareSerialize();
    serialize("");
    changed = false;
}