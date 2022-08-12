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
    struct stat st = {0};

    if (stat(fileName.c_str(), &st) != -1)
    {
        std::string stringMode = Utils::getMode(fileName);
        this->st = st;
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

const std::vector<uint8_t> IndexEntry::getIndexEntryString() const
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
    std::vector<uint8_t> lastcTimeMetadata = Utils::int32ToBytes(st.st_ctim.tv_sec);
    std::vector<uint8_t> cTimeNano = Utils::int32ToBytes(st.st_ctim.tv_nsec);
    std::vector<uint8_t> lastmTimeData = Utils::int32ToBytes(st.st_mtim.tv_sec);
    std::vector<uint8_t> mTimeNano = Utils::int32ToBytes(st.st_mtim.tv_nsec);;
    std::vector<uint8_t> dev = Utils::int32ToBytes(st.st_dev);
    std::vector<uint8_t> ino = Utils::int32ToBytes(st.st_ino);
    std::vector<uint8_t> mode = Utils::int32ToBytes(std::stoul(Utils::getMode(fileName), nullptr, 8));
    std::vector<uint8_t> uid = Utils::int32ToBytes(st.st_uid);
    std::vector<uint8_t> gid = Utils::int32ToBytes(st.st_gid);
    std::vector<uint8_t> fileSize = Utils::int32ToBytes(st.st_size);

    // std::cout << st.st_size << std::endl;
    // for (auto x : fileSize)
    //     std::cout << std::hex << (unsigned int)x << " ";
    // std::cout << std::endl;

    std::vector<uint8_t> result;
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
    std::vector<uint8_t> packedSHA = Utils::packStringToPackedBytes(hash);
    result.insert(result.end(), packedSHA.begin(), packedSHA.end());

    // flags
    result.push_back(static_cast<uint8_t>((flags >> 8) & 0xff));
    result.push_back(static_cast<uint8_t>((flags >> 0) & 0xff));

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
    std::vector<uint8_t> lenBytes(Utils::int32ToBytes(len));
    result.insert(result.end(), lenBytes.begin(), lenBytes.end());

    return result;
}

void Index::prepareSerialize()
{
    /* Might move this block of code later, because I don't want to construct a new Index again each time I add*/

    std::vector<uint8_t> result;
    content.clear(); // we make sure to add all elements again, not over last ones
    for (auto e : entries)
    {
        std::vector<uint8_t> entryString = e.second.getIndexEntryString();
        content.insert(content.end(), entryString.begin(), entryString.end());
    }
    std::string header = getHeader(entries.size());
    result.assign(header.begin(), header.end());
    result.insert(result.end(), content.begin(), content.end());

    hash.assign(Utils::getSHA1hash(result));
}

void Index::removeEntry(const std::string& path)
{
    entries.erase(path);
    // split path into directories
    std::vector<std::string> dirs = Utils::splitPath(path);

    std::string currentDeletedDir = "";
    for (size_t len = dirs.size(), i = 0; i < len - 1; ++i)
    {
        currentDeletedDir += dirs[i];
        directories[currentDeletedDir].erase(path);
        if (directories[currentDeletedDir].empty())
            directories.erase(currentDeletedDir);
        currentDeletedDir += "/";
    }
}

void Index::removeChildren(const std::string& path)
{
    if (directories.find(path) != directories.end())
    {
        std::set<std::string> children = directories[path];
        for (auto x : children)
            removeEntry(x);
    }
}

void Index::discardConflicts(const std::string& file)
{
    // split path into directories
    std::vector<std::string> dirs = Utils::splitPath(file);

    std::string currentDeletedDir = "";
    for (size_t len = dirs.size(), i = 0; i < len - 1; ++i)
    {
        currentDeletedDir += dirs[i];
        removeEntry(currentDeletedDir);
        currentDeletedDir += "/";
    }
    removeChildren(file);
}

void Index::add(const std::vector<std::string>& files)
{
    changed = true;
    for (auto file : files)
    {   
        // make sure there are no files with same
        //  name as directories on the given file path
        discardConflicts(file); 
        entries[file] = IndexEntry(file);
        // std::cout << entries[file].getFileName() << " " << entries[file].getMode() << " " << entries[file].getHash() << std::endl;
    }
    
}

void Index::serialize(const std::string&) const
{
    std::string header = getHeader(entries.size());
    std::vector<uint8_t> v;
    // header
    v.assign(header.begin(), header.end());
    // actual content
    v.insert(v.end(), content.begin(), content.end());
    // SHA1 hash
    std::vector<uint8_t> packedHash = Utils::packStringToPackedBytes(hash);
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
    std::vector<uint8_t> index = Utils::readBinaryFile(Git::gitDir + "/" + "index");
    if (index.size() == 0)
        return;
    if (!checkSignature(std::string(index.begin(), index.begin() + 8)))
    {
        std::cout << "Bad index signature!" << std::endl;
        return;
    }

    uint32_t entryCount = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + 8, index.begin() + 12));
    
    size_t start = 12;
    for (size_t i = 0; i < entryCount; ++i)
    {
        struct stat st;
        st.st_ctim.tv_sec = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_ctim.tv_nsec = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_mtim.tv_sec = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_mtim.tv_nsec = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_dev = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_ino = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_mode  = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_uid = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_gid = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 4));
        start += 4;
        st.st_size = Utils::bytesToInt32(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 4));
        start += 4;

        std::string ieHash = Utils::unpackBytesToString(std::vector<uint8_t>(index.begin() + start, index.begin() + start + 20));
        start += 20;

        uint16_t ieFlags = (index[start] << 8) | index[start + 1];
        start += 2;

        std::string ieFileName = "";
        for (; index[start] != '\0'; ++start)
            ieFileName += index[start];

        start = start + 8 - (start - 12) % 8;
        entries[ieFileName] = IndexEntry(st, ieHash, ieFlags, ieFileName);
        // split path into directories
        std::vector<std::string> dirs = Utils::splitPath(ieFileName);

        std::string currentDir = "";
        for (size_t len = dirs.size(), i = 0; i < len - 1; ++i)
        {
            currentDir += dirs[i];
            // add files to all parent directories
            if (directories.find(currentDir) == directories.end())
                directories[currentDir] = std::set<std::string>({ieFileName});
            else
                directories[currentDir].insert(ieFileName);
            currentDir += "/";
        }
        // std::cout << "Loaded index file: " << ieFileName << " index size : " << st.st_size << std::endl;
    }
    
    if (Utils::getSHA1hash(std::vector<uint8_t>(index.begin(), index.end() - 20))
        != Utils::unpackBytesToString(
            std::vector<uint8_t>(index.end() - 20, index.end())
            ))
                std::cout << "Checksum corrupt !" << std::endl;
}

Index::Index() : Object(), directories(std::map<std::string, std::set<std::string>>()), entries(std::map<std::string, IndexEntry>()), changed(false)
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

void Index::updateEntryStat(const std::string& path, const struct stat& st)
{
    entries[path].setStat(st);
    changed = true;
}

bool Index::isTracked(const std::string& path) const
{
    return (entries.find(path) != entries.end()) 
    || (directories.find(path) != directories.end());
}

bool IndexEntry::statMatch(const struct stat& st) const
{
    return (Utils::getModeFromStat(this->st) == 
            Utils::getModeFromStat(st)) &&
         (this->st.st_size == 0 ||
           this->st.st_size == st.st_size);
}

bool IndexEntry::timesMatch(const struct stat& st) const
{
    return this->st.st_ctim.tv_nsec == st.st_ctim.tv_nsec &&
    this->st.st_ctim.tv_sec == st.st_ctim.tv_sec && 
    this->st.st_mtim.tv_nsec == st.st_mtim.tv_nsec && 
    this->st.st_mtim.tv_sec == st.st_mtim.tv_sec;
}